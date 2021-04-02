/*
  ==============================================================================
    Zhe Deng 2021
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#pragma once

#include "xsimd/xsimd.hpp"
#include "Utility.h"

enum class Multimode1FilterType
{
    lowpass,
    highpass
};

/** Abstract First Order Multimode Filter Class*/
template<typename SampleType>
class Multimode1 
{
public:

    using SIMD = xsimd::simd_type<SampleType>;

    using FilterType = Multimode1FilterType;

    /** Set mode to lowpass or highpass */
    void setMode(FilterType mode) noexcept { filterType = mode; }

    /** Set the processing specifications */
    virtual void prepare(const double sampleRate, const int samplesPerBlock) = 0;

    /** Reset the internal state*/
    virtual void reset() = 0;

    /** Process a single SIMD sample */
    virtual SIMD processSample(SIMD x) noexcept = 0;

    /** Process an interleaved, SIMD-aligned buffer */
    void process(SampleType* interleaved) noexcept
    {
        for (size_t i = 0; i < interleavedSize; i += SIMD::size)
        {
            SIMD x = xsimd::load_aligned(&interleaved[i]);
            xsimd::store_aligned(&interleaved[i], processSample(x));
        }
    }

protected:

    //outputs
    SIMD y;
    
    //parameter
    SIMD G;

    //spec
    size_t interleavedSize;

    //parameters
    FilterType filterType = FilterType::lowpass;

};

/** First-order Multimode Filter 
*
*   Note 1: Use for time domain effects. Left Riemann sum integrator and unit delay feedback resolution.
*/
template<typename SampleType>
class Multimode1_Riemann final : public Multimode1<SampleType>
{
public:

    /** Convert tau in miliseconds to G
    *
    *   Note 1: G is the total integrator gain after prewarping and resolving unit delays.
    *
    *   Note 2: This method doesn't perform checks. Make sure cutoff is below Nyquist before calling.
    */
    xsimd::simd_type<SampleType> tauToG(SampleType tauMs) noexcept;

    /** Set G directly
    *
    *   Note 1: G is the total integrator gain after prewarping and resolving unit delays.
    *
    *   Note 2: This method doesn't perform checks. Make sure cutoff is below Nyquist before calling.
    */
    void setG(SIMD newG) noexcept { G = newG; }

    /** Set the time in miliseconds for the step response to reach 1-1/e */
    void setTau(SampleType tauMs) noexcept;

    /** Convert omega to G
    * 
    *   Note: This function is for programmatically modulating cutoff at audio rates. 
    */
    void setOmega(SIMD omega) noexcept
    {
        G = SIMD(1.0) - xsimd::exp(mT*omega);
    }
    
    void prepare(const double sampleRate, const int samplesPerBlock);

    void reset()
    {
        y = SIMD(0.0);
    }

    SIMD processSample(SIMD x) noexcept
    {
        y = xsimd::fma(G, (x - y), y); //y = G * (x - y) + y
        return filterType == (FilterType::lowpass) ? y : x - y;
    }

private:

    //spec
    SampleType mT, mT1000;
};

/** First Order Multimode Filter using Topology Preserving Transform Filter */
template <typename SampleType>
class Multimode1_TPT final : public Multimode1<SampleType>
{
public:

    /** Set the filter cutoff frequency in hertz 
    *
    *   Note: Do not set cutoff higher than or near Nyquist
    */
    void setCutoff(SampleType cutoffHz) noexcept;

    void prepare(const double sampleRate, const int samplesPerBlock);

    /** Reset the internal state*/
    void reset()
    {
        s = SIMD(0.0);
    }

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

private:
    //spec
    SampleType Tpi;
    SIMD T_2;

    //state
    SIMD s;
};

/** Peak Ballistics Filter */
template <typename SampleType>
class BallisticsFilter
{
public:

    using SIMD = xsimd::simd_type<SampleType>;

    void setAttack(SampleType attackMs) noexcept;

    void setRelease(SampleType releaseMs) noexcept;

    /** Enable or disable stereo linking
    *
    *   Note: Stereo linking replaces detector with the average value between channels 
    */
    void setStereoLink(bool enableStereoLink)
    {
        stereoLink = enableStereoLink;
    }

    void prepare(double sampleRate, int samplesPerBlock);

    void reset()
    {
        MM1.reset();
    }

    /** Process a SIMD register 
    *
    *   Note: Unused channels must be zeroed for stereo link to function
    */
    SIMD processSample(SIMD x) noexcept
    {
        //detector
        detector = xsimd::abs(x);
        //stereo link
        if (stereoLink) detector = xsimd::hadd(detector) / SIMD(Spec::numChannels);
        //branching cutoff
        MM1.setG(xsimd::select(detector < y, Gr, Ga)); // rect < y ? Ga : Gr 
        //filter
        y = MM1.processSample(detector);
        return y;
    }

private:

    //filter
    Multimode1_Riemann<SampleType> MM1;

    //parameters
    SIMD Gr = SIMD(0.5), Ga = SIMD(0.5);
    bool stereoLink = false;

    //outputs
    SIMD detector, y;
};

//TODO SIMD mask MM1 branch
//TODO FMA
//TODO MM1_Riemann omegaToG lookuptable