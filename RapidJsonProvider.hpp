// MIT License
// 
// Copyright(c) 2018 Alex Tversky
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#pragma once

#include "Util/IPairWrittable.hpp"
#include "Util/ILogProvider.hpp"
#include <document.h> // RapidJOSN API
#include <sstream>    // std::stringstream

namespace Util
{
/**
 *   \brief  RapidJsonProvider supports IPairWrittable
 *           and provides key-value getter using RapidJSON API.
 */
class RapidJsonProvider : public IPairWrittable
{
  public:
    explicit RapidJsonProvider(Util::ILogProvider &logger,
                               const std::string &jsonSerializedText)
        : loggerRef_(logger)
    {
        initialized_ = !doc_.Parse<0>(jsonSerializedText.c_str()).HasParseError();
        if (!initialized_)
        {
            loggerRef_.error("Failed to parse input JSON text: " + jsonSerializedText);
        }
    }

  public:
    virtual ~RapidJsonProvider()
    {
    }

    bool getKeyValue(const std::string &key, std::vector<KeyValuePair> &values) override
    {
        return false;
    }

    /**
     * @brief Retrieves string value by key.
     * 
     * @param [in] key - a key (can be multilevel/nested separated by "/" delimiter),
     *                   e.g. "message_bus/subscribe_topic"
     * @param [out] values - a single value stored in values[0]
     * @return true - if key found
     * @return false - if key not found or parsing/initialization failed
     */
    bool getValue(const std::string &key, std::vector<std::string> &values) override
    {
        values.clear();
        if (initialized_ && !key.empty())
        {
            // Tokenize input key
            std::vector<std::string> keyLevels;
            std::istringstream strStream(key);
            std::string keyLevel;
            while (std::getline(strStream, keyLevel, '/'))
            {
                if (!keyLevel.empty())
                {
                    keyLevels.push_back(keyLevel);
                }
            }
            if (keyLevels.empty())
            {
                keyLevels.push_back(key);
            }

            rapidjson::Document cloneDoc;
            cloneDoc.CopyFrom(doc_, cloneDoc.GetAllocator());

            // Traverse DOM with multilevel key
            std::string parsedKey(keyLevels[0]);
            if (cloneDoc.HasMember(parsedKey.c_str()))
            {
                rapidjson::Value& value = cloneDoc[parsedKey.c_str()];
                if (value.IsObject() && keyLevels.size() > 1) // nested key
                {
                    for (int index = 1;
                         index < static_cast<int>(keyLevels.size());
                         ++index)
                    {
                        keyLevel = keyLevels[index];
                        parsedKey += "/" + keyLevel;
                        if (value.HasMember(keyLevel.c_str()))
                        {
                            value = value[keyLevel.c_str()];
                            if (value.IsString())
                            {
                                break;
                            }
                        }
                        else
                        {
                            loggerRef_.warn("No match for nested key: " + parsedKey);
                            break; // key not found
                        }
                    }
                }

                if (value.IsString() && 0 == parsedKey.compare(key))
                {
                    values.push_back(value.GetString());
                    loggerRef_.debug("Found " + parsedKey + std::string("='") +
                                 values[0] + std::string("'"));
                }
            }
        }
        return !values.empty();
    }

    /**
     * @brief Unsupported.
     * 
     * @param key - a key
     * @param value - a value
     * @return false
     */
    bool setValue(const std::string &, const std::string &) override
    {
        return false;
    }

  private:
    RapidJsonProvider() = delete;
    RapidJsonProvider(const RapidJsonProvider &other) = delete;
    RapidJsonProvider(const RapidJsonProvider &&other) = delete;
    RapidJsonProvider &operator=(const RapidJsonProvider &other) = delete;
    RapidJsonProvider &operator=(const RapidJsonProvider &&other) = delete;

  private:
    Util::ILogProvider &loggerRef_;
    rapidjson::Document doc_;
    bool initialized_;
};
} // namespace Util
