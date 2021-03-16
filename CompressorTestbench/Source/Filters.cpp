/*
  ==============================================================================
    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#include "Filters.h"

#pragma region Multimode1_TPT

template<typename SampleType>
void Multimode1_TPT<SampleType>::prepare(const double sampleRate, const int samplesPerBlock)
{
    T500 = 500.0 / sampleRate;
    T_PI = juce::MathConstants<SampleType>::pi / sampleRate;
    interleavedSize = samplesPerBlock*SIMD::size;
}

template<typename SampleType>
void Multimode1_TPT<SampleType>::setAttack(SampleType attackMs) noexcept
{
    SampleType g = tan(T500 / attackMs); //tan((T/2) * w) = tan((T/2) * (1000/attackMs))
    
    G = SIMD(g / (1.0 + g));
}

template<typename SampleType>
void Multimode1_TPT<SampleType>::setCutoff(SampleType cutoffHz) noexcept
{
    SampleType g = tan(T_PI * cutoffHz); //tan((T/2) * w) = tan((T/2) * 2 * PI * cutoffHz)
    G = SIMD(g / (1.0 + g));
}

// "How can I avoid linker errors with my template classes?" https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
template class Multimode1_TPT <float>;
template class Multimode1_TPT <double>;

#pragma endregion

#pragma region BallisticsFilter_IIR

template<typename SampleType>
void BallisticsFilter_IIR<SampleType>::prepare(double sampleRate, int samplesPerBlock) { T_m1k = -1000.0 / sampleRate; }

template<typename SampleType>
void BallisticsFilter_IIR<SampleType>::setAttack(SampleType attackMs) noexcept { a_a = SIMD(exp(T_m1k / attackMs)); } //exp(-w*T) = exp((-1000/attackMs)*T)

template<typename SampleType>
void BallisticsFilter_IIR<SampleType>::setRelease(SampleType releaseMs) noexcept { a_r = SIMD(exp(T_m1k / releaseMs)); }

template class BallisticsFilter_IIR<float>;
template class BallisticsFilter_IIR<double>;

#pragma endregion

#pragma region BallisticsFilter_TPTtype

template<typename SampleType>
void BallisticsFilter_TPTtype<SampleType>::prepare(double sampleRate, int samplesPerBlock) { T500 = 500.0 / sampleRate; }

template<typename SampleType>
void BallisticsFilter_TPTtype<SampleType>::setAttack(SampleType attackMs) noexcept
{
    SampleType g = tan(T500 / attackMs); //tan((T/2) * w) = tan((T/2) * (1000/attackMs))
    Ga = SIMD(g / (1.0 + g));
}

template<typename SampleType>
void BallisticsFilter_TPTtype<SampleType>::setRelease(SampleType releaseMs) noexcept
{
    SampleType g = tan(T500 / releaseMs); //tan((T/2) * w) = tan((T/2) * (1000/attackMs))
    Gr = (g / (1.0 + g));
}

template class BallisticsFilter_TPTtype<float>;
template class BallisticsFilter_TPTtype<double>;

#pragma endregion

#pragma region BallisticsFilter_TPTz

template class BallisticsFilter_TPTz<float>;
template class BallisticsFilter_TPTz<double>;

#pragma endregion

#pragma region BallisticsFilter_TPT

template class BallisticsFilter_TPT<float>;
template class BallisticsFilter_TPT<double>;

#pragma endregion
