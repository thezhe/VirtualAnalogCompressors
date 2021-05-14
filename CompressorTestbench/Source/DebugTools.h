/*
==============================================================================
Tools for debugging and performance profiling.

Currently WINDOWS ONLY

Function calls should be wrapped in '#ifdef DEBUG' and '#endif'

Zhe Deng 2020
thezhefromcenterville@gmail.com

This file is part of CompressorTestBench which is released under the MIT license.
See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
==============================================================================
*/

#pragma once 

#ifdef DEBUG

#include <iostream>
#include <chrono>
#include <atomic>

#if defined(_WIN32)
#include <Windows.h>
#elif defined (__APPLE__)
//MAC OS DEBUG include here
#endif

namespace DebugTools
{

/// <summary>
/// A dummy enum class for testing
/// </summary>
enum class TestEnumClass
{
    one = 1,
    two,
    three
};

/// <summary>
/// Print a C string to debug console
/// </summary>
/// <param name="c_str"></param>
void debugLog(const char* c_str)
{
#if defined(_WIN32)
    OutputDebugStringA(c_str);
#elif defined(//__APPLE__)
    //MAC OS DEBUG log here
#else
    //Linux
#endif 
}

/// <summary>
/// Check if atomic-wrapped types are lock-free
/// </summary>
void isAtomicLockFree()
{
    std::atomic<float> aFloat;
    std::atomic<double> aDouble;
    std::atomic<size_t> aSize_t;
    std::atomic<int> aInt;
    std::atomic<TestEnumClass> aEnumClass;
    std::atomic<bool> aBool;

    std::ostringstream stringStream;
    stringStream << "<atomic> checks==================================================================\n";
    stringStream << "atomic<float>.is_lock_free()\n" << aFloat.is_lock_free() << "\n";
    stringStream << "atomic<double>.is_lock_free()\n" << aDouble.is_lock_free() << "\n";
    stringStream << "atomic<size_t>.is_lock_free()\n" << aSize_t.is_lock_free() << "\n";
    stringStream << "atomic<int>.is_lock_free()\n" << aInt.is_lock_free() << "\n";
    stringStream << "atomic<enum class>.is_lock_free()\n" << aEnumClass.is_lock_free() << "\n";
    stringStream << "atomic<bool>.is_lock_free()\n" << aBool.is_lock_free() << "\n";

    debugLog(stringStream.str().c_str());
}

/// <summary>
/// Calculate the average time between start and stop locations given the number of runs
/// </summary>
class FunctionTimer
{
public:

    using high_resolution_clock = std::chrono::high_resolution_clock;
    using microseconds = std::chrono::microseconds;
    using time_point = std::chrono::steady_clock::time_point;

    void prepare(size_t numRuns)
    {
        debugLog("Starting Function Timer==================================================================\n");
        runs = numRuns;
    }

    void start()
    {
        //get time
        t0 = high_resolution_clock::now();

        //end of runs
        if (currentRun == runs) return;
    }

    void stop()
    {
        //get time
        auto length = std::chrono::duration_cast<microseconds>(high_resolution_clock::now() - t0).count();

        //end of runs
        if (currentRun == runs) return;

        //update total
        totalTime += length;

        //update max/min
        if (length > maxTime)
            maxTime = length;

        if (length < minTime)
            minTime = length;

        //end of runs
        if (++currentRun == runs)
        {
            std::ostringstream stringStream;
            debugLog("Stopping Function Timer==================================================================\n");
            stringStream << "Stats of first " << runs << " runs in microseconds:\n";
            stringStream << "Average: " << int(double(totalTime) / runs) << "\nMax: " << maxTime << "\nMin: " << minTime << "\n";
            std::string outputString = stringStream.str();
            debugLog(stringStream.str().c_str());

        }
    }

    //reset the timer and restart measuring max, min, and average
    void reset()
    {
        currentRun = 0;
        totalTime = 0;
        maxTime = 0;
        minTime = INT32_MAX;
    }

private:

    //results
    long long totalTime = 0, maxTime, minTime = INT32_MAX;

    //temp vars
    time_point t0;
    size_t currentRun = 0;

    //spec
    size_t runs;
};

/// <summary>
/// Find the root of a function f using Newton-Ralphson Method given f(x), df(x)/dx, 
/// an initial guess (x0) and the number of iterations (n). 
/// </summary>
/// <remarks>
/// Used for testing. It is probably better to code a NR routine directly in releases.
/// </remarks>
/// <typeparam name="FnType">Lambda of the form FloatType(FloatType)</typeparam>
/// <param name="f">f(x)</param>
/// <param name="df">df(x)/dx</param>
/// <param name="x0">Initial guess</param>
/// <param name="n">Number of iterations</param>
/// <remarks>
/// Using a lambda to inline custom functions: blog.demofox.org/2015/02/25/avoiding-the-performance-hazzards-of-stdfunction/
/// </remarks>
/// <returns>Numerical approximation of a root a f(x)</returns>
template<typename FnType, typename FloatType>
static constexpr FloatType newtonRalphson(const FnType& f, const FnType& df, FloatType x0, size_t n) noexcept
{
    FloatType y = x0;
    for (size_t i = 0; i < n; ++i)
        y -= f(y) / df(y);
    return y;
}

} // namespace DebugTools

#endif // DEBUG

//TODO OSX, Linux
