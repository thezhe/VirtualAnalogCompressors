/*
  ==============================================================================
    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#pragma once

#include "Filters.h"

#pragma region Inductors

/** Nonlinear RL filter based on modulating cutoff model
*
*   Note 1: Use for time domain effects.
*/
template<typename SampleType>
class RL_Modulating_Riemann
{
public:
    
    using SIMD = xsimd::simd_type<SampleType>;
    
    /** Set the inductor saturation
    *
    *   Note: A good range is [0, 1000] with log tapered controls
    */
    void setSaturation(SampleType saturationConstant) noexcept;

    /** Set the time in miliseconds for the step response to reach 1-1/e when inductor saturation is 0 */
    void setLinearTau(SampleType linearTauMs) noexcept;

    /** Set the processing specifications */
    void prepare(const double sampleRate, const int samplesPerBlock);

    /** Reset the internal state*/
    void reset() 
    {
        MM1.reset();
    }

    /** Process a single SIMD sample */
    SIMD processSample(SIMD x) noexcept
    {
        MM1.setOmega(xsimd::pow(wLinSq + xsimd::abs(y), SIMD(2.0)));
        y = MM1.processSample(x);
        return y;
    }

    /** Process an interleaved, SIMD-aligned buffer */
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
    SampleType interleavedSize;

    //parameters
    SIMD wLinSq, sat;

    //output
    SIMD y;

    //filter
    Multimode1_Riemann<SampleType> MM1;
};

/*
template<typename SampleType>
class RL_Modulating_TPTz
{
public:

    using SIMD = xsimd::simd_type<SampleType>;

   
    void prepare(const double sampleRate, const int samplesPerBlock);


    void setLinearCutoff(SampleType linearCutoffHz) noexcept;


    void setSaturation(SampleType saturationConstant) noexcept;


    void setMode(FilterType mode) { mm1_TPT.setMode(mode); }

    void reset();

    SIMD processSample(SIMD x)
    {
        //update omega
        SIMD temp = a + sat * xsimd::abs(y);
        mm1_TPT.setOmega(xsimd::min(g * temp * temp, omegaLimit));
        //filter
        y = mm1_TPT.processSample(x);
        return y;
    }
private:

    //parameters
    SIMD g = SIMD(1.0), a, sat;
    SIMD linearCutoffRad;

    //spec
    SIMD omegaLimit;

    //state
    SIMD y;

    //filter
    Multimode1_TPT<SampleType> mm1_TPT;

};
*/

#pragma endregion

//TODO RL models Cutoff Mod, Jiles

