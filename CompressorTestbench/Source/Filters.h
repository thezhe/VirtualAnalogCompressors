/*
  ==============================================================================
    Zhe Deng 2021
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "xsimd/xsimd.hpp"

enum class Multimode1TPTFilterType
{
    lowpass,
    highpass
};

/** First Order Multimode Filter using Topology Preserving Transform Filter */
template <typename SampleType>
class Multimode1_TPT
{
public:
    using SIMD = xsimd::simd_type<SampleType>;

    using FilterType = Multimode1TPTFilterType;

    /** Set the internal processing specifications */
    void prepare(const double sampleRate, const int samplesPerBlock);

    /** Set the time for the step response to reach 1-1/e in miliseconds */
    void setAttack(SampleType attackMs) noexcept;

    /** Set the filter cutoff in hertz */
    void setCutoff(SampleType cutoffHz) noexcept;

    /** Process a SIMD register */
    SIMD processSample(SIMD x) noexcept
    {
        //filter
        SIMD v = (x - s) * G;
        y = v + s;
        s = y + v;
        //choose highpass or lowpass
        return (filterType == FilterType::lowpass) ? y : x - y;
    }

    /** Process an interleaved and SIMD-aligned buffer */
    void process(SampleType* interleaved) noexcept
    {
        for (size_t i = 0; i < interleavedSize; i += SIMD::size)
        {
            SIMD x = xsimd::load_aligned(&interleaved[i]);
            xsimd::store_aligned(&interleaved[i], processSample(x));
        }
    }

private:
    //spec
    SampleType T500, T_PI;
    size_t interleavedSize;

    //parameters
    FilterType filterType = FilterType::lowpass;
    SIMD G;

    //outputs
    SIMD y;

    //state
    SIMD s;
};

/** Peak Ballistics Filter using a first order IIR */
template <typename SampleType>
class BallisticsFilter_IIR
{
public:

    using SIMD = xsimd::simd_type<SampleType>;

    void prepare(double sampleRate, int samplesPerBlock);

    void setAttack(SampleType attackMs) noexcept;

    void setRelease(SampleType releaseMs) noexcept;

    /** Process a SIMD register */
    SIMD processSample(SIMD x) noexcept
    {
        //rectifier
        rect = xsimd::abs(x);
        //branching cutoff
        SIMD a = xsimd::select(y > rect, a_r, a_a); //y > rect ? a_r : a_a 
        //filter
        y = (SIMD(1.0) - a) * rect + a * y;
        return y;
    }

private:
    //spec
    SampleType T_m1k;

    //parameters
    SIMD a_r, a_a;

    //outputs
    SIMD rect, y;

    //state
    SIMD s;
};

/** Abstract Class */
template <typename SampleType>
class BallisticsFilter_TPTtype
{
public:

    using SIMD = xsimd::simd_type<SampleType>;

    void prepare(const double sampleRate, const int samplesPerBlock);

    void setAttack(SampleType attackMs) noexcept;

    void setRelease(SampleType releaseMs) noexcept;

    virtual SIMD processSample(SIMD buffer) noexcept = 0;

protected:

    //spec
    SampleType T500;

    //parameters
    SIMD Gr, Ga;

    //outputs
    SIMD rect, y;

    //state
    SIMD s;
};

/** Peak Ballistics Filter using a first order Topology Preserving Transform Filter
    Note: Feedback loop in the branching function contains a unit delay
*/
template <typename SampleType>
class BallisticsFilter_TPTz : public BallisticsFilter_TPTtype<SampleType>
{
public:

    SIMD processSample(SIMD x) noexcept
    {
        //rectifier
        rect = xsimd::abs(x);
        //branching cutoff
        SIMD G = xsimd::select(rect < y, Gr, Ga);
        //filter
        SIMD v = (rect - s) * G;
        y = v + s;
        s = y + v;
        return y;
    }
};

/** Peak Ballistics Filter using a first order Topology Preserving Transform Filter */
template <typename SampleType>
class BallisticsFilter_TPT : public BallisticsFilter_TPTtype<SampleType>
{
public:

    SIMD processSample(SIMD x) noexcept
    {
        //rectifier
        rect = xsimd::abs(x);
        //branching cutoff
        SIMD G = xsimd::select(s > rect, Gr, Ga);
        //filter
        SIMD v = (rect - s) * G;
        y = v + s;
        s = y + v;
        return y;
    }
};

//TODO SIMD mask MM1 branch

