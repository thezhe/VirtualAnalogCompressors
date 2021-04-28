/*
  ==============================================================================
    Tools for debugging and performance profiling.
    Windows only.

    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#pragma once 

#include <iostream>
#include <chrono>
#include <atomic>

#include <Windows.h>


namespace DebugFunctions
{
    /** Print a single object */
    template<typename T>
    void debugLog(T x)
    {
        std::ostringstream stringStream;
        stringStream << x << "\n";

#ifdef _WIN32
        OutputDebugStringA(stringStream.str().c_str());
#endif
    }
}

namespace SystemSpecs
{
  /*  void isAtomicLockFree()
    {
        std::atomic<float> aFloat;
        std::atomic<double> aDouble;
        std::atomic<size_t> aSize_t;
        std::atomic<int> aInt;

        std::ostringstream stringStream;
        stringStream << "<atomic> checks==================================================================\n";
        stringStream << "atomic<float>.is_lock_free()\n" << aFloat.is_lock_free() << "\n";
        stringStream << "atomic<double>.is_lock_free()\n" << aDouble.is_lock_free() << "\n";
        stringStream << "atomic<size_t>.is_lock_free()\n" << aSize_t.is_lock_free() << "\n";
        stringStream << "atomic<int>.is_lock_free()\n" << aInt.is_lock_free() << "\n";

#ifdef _WIN32
        OutputDebugStringA(stringStream.str().c_str());
#endif
    }*/
}

class FunctionTimer
{
public:

    using high_resolution_clock = std::chrono::high_resolution_clock;
    using microseconds = std::chrono::microseconds;
    using time_point = std::chrono::steady_clock::time_point;

    void prepare(size_t numRuns)
    {
        OutputDebugStringA("Starting Function Timer==================================================================\n");
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
            OutputDebugStringA("Stopping Function Timer==================================================================\n");
            stringStream << "Stats of first " << runs << " runs in microseconds:\n";
            stringStream << "Average: " << int(double(totalTime) / runs) << "\nMax: " << maxTime << "\nMin: " << minTime << "\n";
            std::string outputString = stringStream.str();
            OutputDebugStringA(stringStream.str().c_str());
                
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

//TODO windows only
//TODO Atomic check
//TODO onetime print in prepare