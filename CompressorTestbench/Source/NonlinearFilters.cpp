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
void RL_Modulating_Riemann<SampleType>::setSaturation(SampleType saturationConstant) noexcept
{
    sat = SIMD(saturationConstant);
}

template<typename SampleType>
void RL_Modulating_Riemann<SampleType>::setLinearTau(SampleType linearTauMs) noexcept
{
    wLinSq = SIMD(sqrt(1000.0 / linearTauMs));
}

template<typename SampleType>
void RL_Modulating_Riemann<SampleType>::prepare(const double sampleRate, const int samplesPerBlock)
{
    reset();
    omegaLimit = SIMD(MathConstants<SampleType>::twoPi * 0.45 * sampleRate);
    MM1.prepare(sampleRate, samplesPerBlock);
}

template class RL_Modulating_Riemann<float>;
template class RL_Modulating_Riemann<double>;

/*
template<typename SampleType>
void RL_Modulating_TPTz<SampleType>::prepare(const double sampleRate, const int samplesPerBlock)
{
    omegaLimit = SIMD(MathConstants<SampleType>::twoPi * sampleRate * 0.45); //limit omega to slightly below Nyquist
    y = SIMD(0.0);
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

template<typename SampleType>
void RL_Modulating_TPTz<SampleType>::reset()
{
    y = SIMD(0.0);
    mm1_TPT.reset();
}

template class RL_Modulating_TPTz<float>;
template class RL_Modulating_TPTz<double>;
*/