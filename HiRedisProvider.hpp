// Copyright 2018 Alex Tversky
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without
// restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.#pragma once
#pragma once

#include "Util/IPairWrittable.hpp"
#include "Util/ILogProvider.hpp"
#include <hiredis.h> // redis client
#include <mutex>

static const std::string REDIS_HOST("127.0.0.1");
static const int REDIS_PORT(6379);

namespace Util
{

/**
 * @brief get key and value as a pair
 */
KeyValuePair getAsKeyValuePair(std::string key, std::string value)
{
    return KeyValuePair(key, value);
}

/**
 * @brief get only value
 */
std::string getAsValue(std::string key, std::string value)
{
    return value;
}

/**
 *   \brief  HiRedisProvider supports IPairWrittable
 *           and provides key-value getter and setter using hredis API.
 */
class HiRedisProvider : public IPairWrittable
{
  public:
    explicit HiRedisProvider(Util::ILogProvider &logger) : loggerRef_(logger)
    {
        std::lock_guard<std::recursive_mutex> guard(connContextMutex_);

        const struct timeval connectionTimeout = {2, 0}; // 2.0 seconds
        connContext_ = redisConnectWithTimeout(REDIS_HOST.c_str(), REDIS_PORT, connectionTimeout);
        if (connContext_ == nullptr || connContext_->err)
        {
            if (connContext_)
            {
                loggerRef_.error("Failed to connect to redis server, error: " +
                                 std::string(connContext_->errstr));
                redisFree(connContext_);
            }
            else
            {
                loggerRef_.error("Failed to connect to redis server, can't allocate redis context!");
            }
        }
    }

  public:
    virtual ~HiRedisProvider()
    {
        if (isConnected())
        {
            redisFree(connContext_);
        }
    }

    /**
     * @brief Retrieves value/s by key, as Key/Value pairs
     *
     * @param [in] key - a key (can have "\*" notation, e.g. "printer/terminal/\*")
     * @param [out] values - a single or multiple values
     * @return true - if key found
     * @return false - if key not found
     */
    bool getKeyValue(const std::string &key, std::vector<KeyValuePair> &values) override
    {
        return getAndProcessValue<KeyValuePair>(key, values, &Util::getAsKeyValuePair);
    }

    /**
     * @brief Retrieves value/s by key.
     *
     * @param [in] key - a key (can have "\*" notation, e.g. "printer/terminal/\*")
     * @param [out] values - a single or multiple values
     * @return true - if key found
     * @return false - if key not found
     */
    bool getValue(const std::string &key, std::vector<std::string> &values) override
    {
        return getAndProcessValue<std::string>(key, values, &Util::getAsValue);
    }

    /**
     * @brief Sets value for given key into connected redis.
     *
     * @param key - a key
     * @param value - a value
     * @return true - if succeded, false - otherwise
     */
    bool setValue(const std::string &key, const std::string &value) override
    {
        bool status(false);
        if (isConnected())
        {
            std::lock_guard<std::recursive_mutex> guard(connContextMutex_);

            void *replyRawObj = redisCommand(connContext_, "SET %s %s", key.c_str(), value.c_str());
            redisReply *replyObj = reinterpret_cast<redisReply *>(replyRawObj);
            loggerRef_.trace(std::string("Redis replied for SET ") + key + std::string(" ") + value +
                             std::string(": ") + (replyObj->str ? replyObj->str : std::string()));
            freeReplyObject(replyObj);
            status = true;
        }
        else
        {
            loggerRef_.error("Redis is not connected! Cannot set any values!");
        }
        return status;
    }

  protected:
    /**
     * @brief Validates if redis connection opened?
     *
     * @return true - if opened
     * @return false - if not opened
     */
    bool isConnected()
    {
        std::lock_guard<std::recursive_mutex> guard(connContextMutex_);
        return (connContext_ != nullptr);
    }

    /**
     * @brief Retrieves value based on unique key.
     * 
     * @param [in] key - a key
     * @param [out] value - a value
     * @return true - if key found
     * @return false - if key not found
     */
    bool getValue(const std::string &key, std::string &value)
    {
        if (isConnected() && !key.empty())
        {
            std::lock_guard<std::recursive_mutex> guard(connContextMutex_);

            void *replyRawObj = redisCommand(connContext_, "GET %s", key.c_str());
            redisReply *replyObj = reinterpret_cast<redisReply *>(replyRawObj);
            value.assign(replyObj->str ? replyObj->str : std::string());
            loggerRef_.trace(std::string("Redis replied for GET ") + key +
                             std::string(": ") + value);
            freeReplyObject(replyObj);
        }
        return !value.empty();
    }

  private:
    HiRedisProvider() = delete;
    HiRedisProvider(const HiRedisProvider &other) = delete;
    HiRedisProvider(const HiRedisProvider &&other) = delete;
    HiRedisProvider &operator=(const HiRedisProvider &other) = delete;
    HiRedisProvider &operator=(const HiRedisProvider &&other) = delete;

  private:
    Util::ILogProvider &loggerRef_;
    redisContext *connContext_;
    std::recursive_mutex connContextMutex_;

    /**
     * @brief Retrieves value/s by key and processes them by the given callback
     * @param [in] key - a key (can have "\*" notation, e.g. "printer/terminal/\*")
     * @param [out] values - a single or multiple values
     * @param [in] callback - a callback used for processing of retrieved key value pairs
     * @return true - if key found
     * @return false - if key not found
     */
    template <typename T>
    bool getAndProcessValue(const std::string &key, std::vector<T> &values, std::function<T(std::string, std::string)> callback)
    {
        values.clear();
        if (isConnected() && !key.empty())
        {
            if ('*' != key.back()) // single key
            {
                std::string value;
                if (getValue(key, value))
                {
                    values.push_back(callback(key, value));
                }
            }
            else // asterisk: multiple keys
            {
                std::lock_guard<std::recursive_mutex> guard(connContextMutex_);

                void *replyRawObj = redisCommand(connContext_, "KEYS %s", key.c_str());
                redisReply *replyObj = reinterpret_cast<redisReply *>(replyRawObj);
                loggerRef_.trace(std::string("Redis replied for KEYS ") + key +
                                 std::string(": returns elements: ") + std::to_string(replyObj->elements));
                for (size_t index = 0; index < replyObj->elements; ++index)
                {
                    std::string subkey(replyObj->element[index]->str);
                    std::string value;
                    if (getValue(subkey, value))
                    {
                        values.push_back(callback(subkey, value));
                    }
                }
                freeReplyObject(replyObj);
            }
        }
        return !values.empty();
    }
};
} // namespace Util
