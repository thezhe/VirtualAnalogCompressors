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

template<typename SampleType>
class RL_Modulating_TPTz
{
public:

    using SIMD = xsimd::simd_type<SampleType>;

    /** Set the internal processing specifications */
    void prepare(const double sampleRate, const int samplesPerBlock);

    /** Set the time for the step response to reach 1-1/e when input is zero in miliseconds */
    void setLinearAttack(SampleType attackMs) noexcept;

    /** Set the filter cutoff when input is zero in Hz */
    void setLinearCutoff(SampleType linearCutoffHz) noexcept;

    /** Set the internal saturation constant
    *   
    *   Note: A good range is [0, 1000] with log tapered controls
    */
    void setSaturation(SampleType saturationConstant) noexcept;

    /** Set the internal intensity in dB */
    void setIntensity(SampleType intensitydB) noexcept;
    
    /** Set lowpass or highpass mode */
    void setMode(Multimode1TPTFilterType mode) { mm1_TPT.setMode(mode); }

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

#pragma endregion

//TODO RL models Cutoff Mod, Jiles

