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

#include <string>

namespace Util
{
    namespace Literals
    {
        static const std::string allTag("all");
        static const std::string traceTag("trace");
        static const std::string debugTag("debug");
        static const std::string infoTag("info");
        static const std::string warnTag("warn");
        static const std::string errorTag("error");
        static const std::string fatalTag("fatal");
        static const std::string offTag("off");
    }

    /**
    *   \brief  Logger levels similar to APIs like Log4j
    *
    *   \details  LL_ALL - logs always, LL_OFF - logs never
    */
    enum LogLevel
    {
        LL_ALL = 0x7F,
        LL_TRACE = 0x3F,
        LL_DEBUG = 0x1F,
        LL_INFO = 0xF,
        LL_WARN = 0x7,
        LL_ERROR = 0x3,
        LL_FATAL = 0x1,
        LL_OFF = 0x0
    };

    /**
    *   \brief  ILogProvider is an interface provided logging functionality
    *
    *   \details  Logging levels similar to APIs like Log4j
    */
    class ILogProvider
    {
    public:
        virtual ~ILogProvider() {}

        virtual bool setLevel(const int level) = 0;
        inline bool setLevel(const std::string& level);
        inline bool setLevel(const std::wstring& level);

        virtual bool log(const std::string& message) = 0;   // ALL
        inline bool log(const std::wstring& message);

        virtual bool trace(const std::string& message) = 0; // TRACE
        inline bool trace(const std::wstring& message);

        virtual bool debug(const std::string& message) = 0; // DEBUG
        inline bool debug(const std::wstring& message);

        virtual bool info(const std::string& message) = 0;  // INFO
        inline bool info(const std::wstring& message);

        virtual bool warn(const std::string& message) = 0;  // WARN
        inline bool warn(const std::wstring& message);

        virtual bool error(const std::string& message) = 0; // ERROR
        inline bool error(const std::wstring& message);

        virtual bool fatal(const std::string& message) = 0; // FATAL
        inline bool fatal(const std::wstring& message);
    };

    /**
    *  \brief Sets logging level
    */
    bool ILogProvider::setLevel(const std::string& level)
    {
        int intLevel(LL_ALL); // all is default

        if( 0 == level.compare(Literals::traceTag) )
        {
            intLevel = LL_TRACE;
        }
        else if( 0 == level.compare(Literals::debugTag) )
        {
            intLevel = LL_DEBUG;
        }
        else if( 0 == level.compare(Literals::infoTag) )
        {
            intLevel = LL_INFO;
        }
        else if( 0 == level.compare(Literals::warnTag) )
        {
            intLevel = LL_WARN;
        }
        else if( 0 == level.compare(Literals::errorTag) )
        {
            intLevel = LL_ERROR;
        }
        else if( 0 == level.compare(Literals::fatalTag) )
        {
            intLevel = LL_FATAL;
        }
        else if( 0 == level.compare(Literals::offTag) )
        {
            intLevel = LL_OFF;
        }

        return setLevel(intLevel);
    }

    /**
    *  \brief Wide string set level
    */
    bool ILogProvider::setLevel(const std::wstring& level)
    {
        std::string asciiLevel(level.begin(), level.end());
        return setLevel(asciiLevel);
    }

    /**
     *  \brief Wide string log
     */
    bool ILogProvider::log(const std::wstring& message)
    {
        std::string asciiMessage(message.begin(), message.end());
        return log(asciiMessage);
    }

    /**
    *  \brief Wide string trace
    */
    bool ILogProvider::trace(const std::wstring& message)
    {
        std::string asciiMessage(message.begin(), message.end());
        return trace(asciiMessage);
    }

    /**
    *  \brief Wide string debug
    */
    bool ILogProvider::debug(const std::wstring& message)
    {
        std::string asciiMessage(message.begin(), message.end());
        return debug(asciiMessage);
    }

    /**
    *  \brief Wide string info
    */
    bool ILogProvider::info(const std::wstring& message)
    {
        std::string asciiMessage(message.begin(), message.end());
        return info(asciiMessage);
    }

    /**
    *  \brief Wide string warn
    */
    bool ILogProvider::warn(const std::wstring& message)
    {
        std::string asciiMessage(message.begin(), message.end());
        return warn(asciiMessage);
    }

    /**
    *  \brief Wide string error
    */
    bool ILogProvider::error(const std::wstring& message)
    {
        std::string asciiMessage(message.begin(), message.end());
        return error(asciiMessage);
    }

    /**
    *  \brief Wide string fatal
    */
    bool ILogProvider::fatal(const std::wstring& message)
    {
        std::string asciiMessage(message.begin(), message.end());
        return fatal(asciiMessage);
    }
}
