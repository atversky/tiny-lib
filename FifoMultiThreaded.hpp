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

#include <list>
#include <mutex>
#include <queue>
#include <string>

namespace Util
{
/**
*  \brief FifoMultiThreaded - a template class wrapping STL queue into
*         multi-threaded FIFO queue API
*/
template <class T = std::string, class A = std::list<T>>
class FifoMultiThreaded
{
  public:
    FifoMultiThreaded();
    virtual ~FifoMultiThreaded();

    bool push(const T &element);
    bool pop(T &element);
    bool empty() const;
    unsigned long long size() const;

  private:
    FifoMultiThreaded(const FifoMultiThreaded &other) = delete;
    FifoMultiThreaded(const FifoMultiThreaded &&other) = delete;
    FifoMultiThreaded &operator=(const FifoMultiThreaded &other) = delete;
    FifoMultiThreaded &operator=(const FifoMultiThreaded &&other) = delete;

  private:
    std::queue<T, A> queue_;
    mutable std::recursive_mutex mutex_;
};

template <class T, class A>
FifoMultiThreaded<T, A>::FifoMultiThreaded()
{
}

template <class T, class A>
FifoMultiThreaded<T, A>::~FifoMultiThreaded()
{
}

template <class T, class A>
bool FifoMultiThreaded<T, A>::push(const T &element)
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    queue_.push(element);
    return true;
}

template <class T, class A>
bool FifoMultiThreaded<T, A>::pop(T &element)
{
    bool status(false);
    {
        std::lock_guard<std::recursive_mutex> guard(mutex_);
        if (!queue_.empty())
        {
            element = queue_.front();
            queue_.pop();
            status = true;
        }
    }

    return status;
}

template <class T, class A>
bool FifoMultiThreaded<T, A>::empty() const
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    return queue_.empty();
}

template <class T, class A>
unsigned long long FifoMultiThreaded<T, A>::size() const
{
    std::lock_guard<std::recursive_mutex> guard(mutex_);
    return queue_.size();
}
}
