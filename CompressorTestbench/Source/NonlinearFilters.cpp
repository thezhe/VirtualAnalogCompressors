/*
  ==============================================================================
    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#include "NonlinearFilters.h"


template<typename SampleType>
inline void RL_Modulating_TPTz<SampleType>::prepare(const double sampleRate, const int samplesPerBlock)
{
    omegaLimit = SIMD(juce::MathConstants<SampleType>::twoPi * sampleRate * 0.45); //limit omega to slightly below Nyquist
    mm1_TPT.prepare(sampleRate, samplesPerBlock);
}

template<typename SampleType>
void RL_Modulating_TPTz<SampleType>::setLinearAttack(SampleType attackMs) noexcept
{
    linearCutoffRad = SIMD(1000.0 / attackMs);
    a = xsimd::sqrt(linearCutoffRad / g);
}

template<typename SampleType>
void RL_Modulating_TPTz<SampleType>::setLinearCutoff(SampleType linearCutoffHz) noexcept
{
    linearCutoffRad = SIMD(juce::MathConstants<SampleType>::twoPi * linearCutoffHz);
    a = xsimd::sqrt(linearCutoffRad / g);
}

template<typename SampleType>
void RL_Modulating_TPTz<SampleType>::setSaturation(SampleType saturationConstant) noexcept
{
    sat = SIMD(saturationConstant);
}

template<typename SampleType>
void RL_Modulating_TPTz<SampleType>::setIntensity(SampleType intensitydB) noexcept
{
    g = SIMD(juce::Decibels::decibelsToGain(intensitydB));
    a = xsimd::sqrt(linearCutoffRad / g);
}

template class RL_Modulating_TPTz<float>;
template class RL_Modulating_TPTz<double>;
