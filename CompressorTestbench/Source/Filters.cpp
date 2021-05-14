/*
==============================================================================
Zhe Deng 2020
thezhefromcenterville@gmail.com

This file is part of CompressorTestBench which is released under the MIT license.
See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
==============================================================================
*/

#include "Filters.h"

namespace VA
{

#pragma region Multimode1

template<typename SampleType>
void Multimode1<SampleType>::setCutoff(SampleType cutoffHz) noexcept
{
    setOmega(MathConstants<SampleType>::pi2 * cutoffHz);
}

template<typename SampleType>
void Multimode1<SampleType>::setTau(SampleType tauMs) noexcept
{
    setOmega(SampleType(1000.0) / tauMs);
}

template<typename SampleType>
SampleType Multimode1<SampleType>::tauToG(SampleType tauMs) noexcept
{
    SampleType g = tan(Tdiv2 * SampleType(1000.0) / tauMs);
    return g / (SampleType(1.0) + g);
}

template<typename SampleType>
void Multimode1<SampleType>::reset()
{
    I1.reset();
}

template<typename SampleType>
void Multimode1<SampleType>::prepare(SampleType sampleRate, size_t numInputChannels)
{
    //filter
    I1.prepare(numInputChannels);
    
    //spec
    Tdiv2 = SampleType(0.5) / sampleRate;
}

template class Multimode1 <float>;
template class Multimode1 <double>;

#pragma endregion

#pragma region BallisticsFilter

template<typename SampleType>
void BallisticsFilter<SampleType>::setAttack(SampleType attackMs) noexcept
{
    Ga = mm1.tauToG(attackMs);
}

template<typename SampleType>
void BallisticsFilter<SampleType>::setRelease(SampleType releaseMs) noexcept
{
    Gr = mm1.tauToG(releaseMs);
}

template<typename SampleType>
void BallisticsFilter<SampleType>::reset()
{
    mm1.reset();
}


template<typename SampleType>
void BallisticsFilter<SampleType>::prepare(SampleType sampleRate, size_t numInputChannels)
{
    mm1.prepare(sampleRate, numInputChannels);
}

template class BallisticsFilter<float>;
template class BallisticsFilter<double>;

#pragma endregion

#pragma region MonoConverter

template <typename SampleType>
void MonoConverter<SampleType>::prepare(size_t numInputChannels)
{
    numChannels = numInputChannels;
    divNumChannels = SampleType(1.0) / numInputChannels;
}

template class MonoConverter<float>;
template class MonoConverter<double>;

#pragma endregion

#pragma region KFilter

template<typename SampleType>
void KFilter<SampleType>::reset()
{
    std::fill(_x1.begin(), _x1.end(), 0.0);
    std::fill(_x2.begin(), _x2.end(), 0.0);
    std::fill(_y1.begin(), _y1.end(), 0.0);
    std::fill(_y2.begin(), _y2.end(), 0.0);
}

template<typename SampleType>
void KFilter<SampleType>::prepare(SampleType sampleRate, size_t numInputChannels)
{
    //Parameters
    SampleType Vh = SampleType(1.58), Vb = SampleType(1.26), Vl = SampleType(1);
    SampleType Q = SampleType(0.71);
    SampleType fc = SampleType(1681.97);

    //Prewarped angular cutoff
    SampleType g = tan(VA::MathConstants<SampleType>::pi * fc / sampleRate);
    SampleType gSq = g * g;

    //Calculate Coefficients
    a1 = 2 * (gSq - 1);
    a2 = gSq - g / Q + 1;
    b0 = Vl * gSq + Vb * g / Q + Vh;
    b1 = 2 * (Vl * gSq - Vh);
    b2 = Vl * gSq - Vb * g / Q + Vh;

    //Normalize to a0
    SampleType a0 = gSq + g / Q + 1;
    a1 /= a0;
    a2 /= a0;
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;

    //allocate state
    _x1.resize(numInputChannels);
    _x2.resize(numInputChannels);
    _y1.resize(numInputChannels);
    _y2.resize(numInputChannels);

    reset();
}

template class KFilter<float>;
template class KFilter<double>;

#pragma endregion

#pragma region Detector

template<typename SampleType>
void Detector<SampleType>::reset()
{
    kFilter.reset();
}

template<typename SampleType>
void Detector<SampleType>::prepare(SampleType sampleRate, size_t samplesPerBlock, size_t numInputChannels)
{
    kFilter.prepare(sampleRate, numInputChannels);
}

template class Detector<float>;
template class Detector<double>;

#pragma endregion

} // namespace VA