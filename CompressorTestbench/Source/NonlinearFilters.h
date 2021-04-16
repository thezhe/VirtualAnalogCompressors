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

/** Nonlinear RL low-pass filter based on modulating cutoff model
*
*   Note: Use for time domain effects. Implementation is based on
*   left Riemann sum integrator and unit delay feedback resolution.
*/
template<typename SampleType>
class RL_Modulating_Riemann : public Processor<SampleType>
{
public:
    
    /** Set the inductor saturation
    *
    *   Note: A good range is [0, 500] with log tapered controls
    */
    void setSaturation(SampleType saturationConstant) noexcept;

    /** Set the time in miliseconds for the step response to reach 1-1/e when inductor saturation is 0 */
    void setLinearTau(SampleType linearTauMs) noexcept;

    /** Set the processing specifications */
    void prepare(double sampleRate, int samplesPerBlock, int numInputChannels);

    /** Reset the internal state*/
    void reset() 
    {
        LP1.reset();
    }

    /** Process a single SIMD sample */
    SIMD processSample(SIMD x) noexcept
    {
        LP1.setOmega(xsimd::min(xsimd::pow(wLinSq + sat*xsimd::abs(y), SIMD(2.0)), omegaLimit));
        y = LP1.processSample(x);
        return y;
    }

private:

    //spec
    SIMD omegaLimit;

    //parameters
    SIMD wLinSq, sat;

    //output
    SIMD y;

    //filter
    LP1_Riemann<SampleType> LP1;
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

//TODO RL models Cutoff Mod TPT, Jiles

