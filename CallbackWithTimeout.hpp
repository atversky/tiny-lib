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

#include <atomic>
#include <chrono>
#include <functional>
#include <mutex>
#include <thread>

namespace Util
{
    /**
    *   \brief  CallbackWithTimeout serves thread safe callback executions.
    */
    template<class ParamType = void>
    class CallbackWithTimeout
    {
        typedef void(*CallbackFunction)(ParamType*);

    public:
        /**
         *  \brief Constructor
         */
        CallbackWithTimeout() noexcept
            : execute_(false)
        {
        }

        /**
        *  \brief Destructor
        */
        virtual ~CallbackWithTimeout() noexcept
        {
            if( execute_.load(std::memory_order_acquire) )
            {
                stop();
            };
        }

        /**
        *  \brief Stops thread execution
        */
        void stop() noexcept
        {
            execute_.store(false, std::memory_order_release);
            if( thread_.joinable() )
            {
                thread_.join();
            }
        }

        /**
        *  \brief Starts immediate thread execution
        *
        *  \param [in] callback - a callback function (static)
        *  \param [in] param - a callback parameter
        */
        void start(CallbackFunction callback, ParamType* param) noexcept
        {
            this->start(0, callback, param);
        }

        /**
        *  \brief Starts immediate thread execution
        *
        *  \param [in] timeoutMilliseconds - a timeout between callback calls
        *  \param [in] callback - a callback function (static)
        *  \param [in] param - a callback parameter
        *
        *  \details Callback could be a blocking call. In cases callback blocked
        *           a thread guarantied running until the callback released.
        *           Note: Not advised to make a blocking callback!
        */
        void start(long long timeoutMilliseconds,
                   CallbackFunction callback,
                   ParamType* param) noexcept
        {
            if( execute_.load(std::memory_order_acquire) )
            {
                stop();
            };

            execute_.store(true, std::memory_order_release);

            thread_ = std::thread([this, timeoutMilliseconds, callback, param]()
            {
                // Exits when stop flag is true
                while (execute_.load(std::memory_order_acquire))
                {
                    // Calls static executor-function
                    callback(param);

                    // Yield context switch and sleeps amount of milliseconds
                    const long timeSliceMilliseconds(25);
                    const long long maxSteps = timeoutMilliseconds / timeSliceMilliseconds;
                    long step(0);
                    while (execute_.load(std::memory_order_acquire) && step++ < maxSteps)
                    {
                        std::this_thread::sleep_for(
                            std::chrono::milliseconds(timeSliceMilliseconds));
                    }
                }
            });
        }

        /**
        *  \brief Validates if thread currently running
        */
        bool isRunning() const noexcept
        {
            return (execute_.load(std::memory_order_acquire) && thread_.joinable());
        }

    private:
        std::atomic<bool> execute_;
        std::thread thread_;
    };
}
