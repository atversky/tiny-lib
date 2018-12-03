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

#include "Util/ILogProvider.hpp"
#include <iostream>
#include <mutex>
#include <string>

#include <log4cpp/Portability.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/Configurator.hh>
#include <log4cpp/PropertyConfigurator.hh>
#include <log4cpp/RollingFileAppender.hh>

namespace Util
{
/**
 * @brief Implements ILogProvider with log4cpp API.
 * 
 */
class Log4CppLoggerProvider : public ILogProvider
{
  public:
    explicit Log4CppLoggerProvider(const std::string &propertiesFile)
        : propertiesFile_(propertiesFile), started_(false)
    {
    }

  public:
    virtual ~Log4CppLoggerProvider() { this->shutdown(); }

    bool log(const std::string &message) override;
    bool trace(const std::string &message) override;
    bool debug(const std::string &message) override;
    bool info(const std::string &message) override;
    bool warn(const std::string &message) override;
    bool error(const std::string &message) override;
    bool fatal(const std::string &message) override;

    bool start();

  protected:
    void shutdown();

  private:
    bool setLevel(const int level) override;

  private:
    Log4CppLoggerProvider() = delete;
    Log4CppLoggerProvider(const Log4CppLoggerProvider &other) = delete;
    Log4CppLoggerProvider(const Log4CppLoggerProvider &&other) = delete;
    Log4CppLoggerProvider &operator=(const Log4CppLoggerProvider &other) = delete;
    Log4CppLoggerProvider &operator=(const Log4CppLoggerProvider &&other) = delete;

  private:
    const std::string propertiesFile_;
    bool started_;
    std::mutex lock_;
};

/**
 * @brief Oevrrides log4cpp root category priority
 * 
 * @param level - LogLevel value
 * @return true - if update succeded
 * @return false - if failed
 */
inline bool Log4CppLoggerProvider::setLevel(const int level)
{
    if (!started_)
    {
        std::cerr << "Call start() first!" << std::endl;
        return false;
    }

    log4cpp::Priority::PriorityLevel log4cppPriority(log4cpp::Priority::NOTSET);
    switch (level)
    {
    case LL_DEBUG:
        log4cppPriority = log4cpp::Priority::DEBUG;
        break;
    case LL_INFO:
        log4cppPriority = log4cpp::Priority::INFO;
        break;
    case LL_WARN:
        log4cppPriority = log4cpp::Priority::WARN;
        break;
    case LL_ERROR:
        log4cppPriority = log4cpp::Priority::ERROR;
        break;
    case LL_FATAL:
        log4cppPriority = log4cpp::Priority::FATAL;
        break;
    }

    if (!started_)
    {
        std::lock_guard<std::mutex> guard(lock_);

        try
        {
            log4cpp::Category::setRootPriority(log4cppPriority);
        }
        catch (log4cpp::ConfigureFailure &e)
        {
            std::cerr << "Set priotity problem " << e.what() << std::endl;
            return false;
        }
    }

    return true;
}

/**
 * @brief Emits ALL-level log
 * 
 * @param message - logging message
 * @return true - if succeded
 * @return false - if log4cpp subsystem not initialized yet
 */
inline bool Log4CppLoggerProvider::log(const std::string &message)
{
    return debug(message);
}

/**
 * @brief Emits TRACE-level log
 * 
 * @param message - logging message
 * @return true - if succeded
 * @return false - if log4cpp subsystem not initialized yet
 */
inline bool Log4CppLoggerProvider::trace(const std::string &message)
{
    return debug(message);
}

/**
 * @brief Emits DEBUG-level log
 * 
 * @param message - logging message
 * @return true - if succeded
 * @return false - if log4cpp subsystem not initialized yet
 */
inline bool Log4CppLoggerProvider::debug(const std::string &message)
{
    if (!started_)
    {
        std::cerr << "Call start() first!" << std::endl;
        return false;
    }

    std::lock_guard<std::mutex> guard(lock_);

    log4cpp::Category &logger = log4cpp::Category::getRoot();
    logger.debug(message);

    return true;
}

/**
 * @brief Emits INFO-level log
 * 
 * @param message - logging message
 * @return true - if succeded
 * @return false - if log4cpp subsystem not initialized yet
 */
inline bool Log4CppLoggerProvider::info(const std::string &message)
{
    if (!started_)
    {
        std::cerr << "Call start() first!" << std::endl;
        return false;
    }

    std::lock_guard<std::mutex> guard(lock_);

    log4cpp::Category &logger = log4cpp::Category::getRoot();
    logger.info(message);

    return true;
}

/**
 * @brief Emits WARN-level log
 * 
 * @param message - logging message
 * @return true - if succeded
 * @return false - if log4cpp subsystem not initialized yet
 */
inline bool Log4CppLoggerProvider::warn(const std::string &message)
{
    if (!started_)
    {
        std::cerr << "Call start() first!" << std::endl;
        return false;
    }

    std::lock_guard<std::mutex> guard(lock_);

    log4cpp::Category &logger = log4cpp::Category::getRoot();
    logger.warn(message);

    return true;
}

/**
 * @brief Emits ERROR-level log
 * 
 * @param message - logging message
 * @return true - if succeded
 * @return false - if log4cpp subsystem not initialized yet
 */
inline bool Log4CppLoggerProvider::error(const std::string &message)
{
    if (!started_)
    {
        std::cerr << "Call start() first!" << std::endl;
        return false;
    }

    std::lock_guard<std::mutex> guard(lock_);

    log4cpp::Category &logger = log4cpp::Category::getRoot();
    logger.error(message);

    return true;
}

/**
 * @brief Emits FATAL-level log
 * 
 * @param message - logging message
 * @return true - if succeded
 * @return false - if log4cpp subsystem not initialized yet
 */
inline bool Log4CppLoggerProvider::fatal(const std::string &message)
{
    if (!started_)
    {
        std::cerr << "Call start() first!" << std::endl;
        return false;
    }

    std::lock_guard<std::mutex> guard(lock_);

    log4cpp::Category &logger = log4cpp::Category::getRoot();
    logger.fatal(message);

    return true;
}

/**
* @brief Starts log4cpp subsystem (initialized with properties file)
* 
* @return true - if initialized
* @return false - false if failed initialized
*/
inline bool Log4CppLoggerProvider::start()
{
    if (!started_)
    {
        std::lock_guard<std::mutex> guard(lock_);
        try
        {
            log4cpp::PropertyConfigurator::configure(propertiesFile_);
        }
        catch (log4cpp::ConfigureFailure &e)
        {
            std::cerr << "Configure problem " << e.what() << std::endl;
            return false;
        }
        started_ = true;
    }

    return started_;
}

/**
* @brief Stops log4cpp subsystem
*/
inline void Log4CppLoggerProvider::shutdown()
{
    if (started_)
    {
        std::lock_guard<std::mutex> guard(lock_);
        try
        {
            log4cpp::Category::shutdown();
        }
        catch (log4cpp::ConfigureFailure &e)
        {
            std::cerr << "Shutdown problem " << e.what() << std::endl;
        }
        started_ = false;
    }
}
}
