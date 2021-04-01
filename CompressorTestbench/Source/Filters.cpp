/*
  ==============================================================================
    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#include "Filters.h"

#pragma region Multimode1_Riemann

template<typename SampleType>
xsimd::simd_type<SampleType> Multimode1_Riemann<SampleType>::tauToG(SampleType tauMs) noexcept
{
    return SIMD(1.0 - exp(mT1000 / tauMs)); //1-e^(1 / (sampleRate * tau)) = 1-e^(T*1000/tauMs) 
}

template<typename SampleType>
void Multimode1_Riemann<SampleType>::setTau(SampleType tauMs) noexcept
{
    setG(tauToG(tauMs));
}

template<typename SampleType>
void Multimode1_Riemann<SampleType>::prepare(const double sampleRate, const int samplesPerBlock)
{
    reset();
    mT = -1.0 / sampleRate;
    mT1000 = -1000.0 / sampleRate;
    interleavedSize = SIMD::size * samplesPerBlock;
}

// "How can I avoid linker errors with my template classes?" 
//https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
template class Multimode1_Riemann <float>;
template class Multimode1_Riemann <double>;

#pragma endregion

#pragma region Multimode1_TPT

template<typename SampleType>
void Multimode1_TPT<SampleType>::prepare(const double sampleRate, const int samplesPerBlock)
{
    reset();
    Tpi = MathConstants<SampleType>::pi / sampleRate;
    interleavedSize = samplesPerBlock*SIMD::size;
}

template<typename SampleType>
void Multimode1_TPT<SampleType>::setCutoff(SampleType cutoffHz) noexcept
{
    SampleType g = tan(Tpi * cutoffHz);
    G = SIMD(g / (1.0 + g));
}

template class Multimode1_TPT <float>;
template class Multimode1_TPT <double>;

#pragma endregion

#pragma region BallisticsFilter

template<typename SampleType>
void BallisticsFilter<SampleType>::prepare(double sampleRate, int samplesPerBlock)
{
    reset();
    MM1.prepare(sampleRate, samplesPerBlock);
}

template<typename SampleType>
void BallisticsFilter<SampleType>::setAttack(SampleType attackMs) noexcept
{
    Ga = MM1.tauToG(attackMs);
}

template<typename SampleType>
void BallisticsFilter<SampleType>::setRelease(SampleType releaseMs) noexcept
{
    Gr = MM1.tauToG(releaseMs);
}


template class BallisticsFilter<float>;
template class BallisticsFilter<double>;

#pragma endregion
