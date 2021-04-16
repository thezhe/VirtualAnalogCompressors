/*
  ==============================================================================
    Tools for debugging and performance profiling.

    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#pragma once 

#include <iostream>
#include <chrono>
#include <Windows.h>

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
                OutputDebugStringA(outputString.c_str());
                
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