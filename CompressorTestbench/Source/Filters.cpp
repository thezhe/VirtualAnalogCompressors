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

template<typename FloatType>
void Multimode1<FloatType>::setCutoff(FloatType cutoffHz) noexcept
{
    setOmega(MathConstants<FloatType>::pi2 * cutoffHz);
}

template<typename FloatType>
void Multimode1<FloatType>::setTau(FloatType tauMs) noexcept
{
    setOmega(FloatType(1000.0) / tauMs);
}

template<typename FloatType>
FloatType Multimode1<FloatType>::tauToG(FloatType tauMs) noexcept
{
    FloatType g = tan(Tdiv2 * FloatType(1000.0) / tauMs);
    return g / (FloatType(1.0) + g);
}

template<typename FloatType>
void Multimode1<FloatType>::reset()
{
    I1.reset();
}

template<typename FloatType>
void Multimode1<FloatType>::prepare(FloatType sampleRate, size_t numInputChannels)
{
    //filter
    I1.prepare(numInputChannels);
    
    //spec
    Tdiv2 = FloatType(0.5) / sampleRate;

    //LUT
    FloatType OmegaLimit = MathConstants<FloatType>::pi2 * sampleRate * 0.499;
    OmegaToGLUT.prepare
    (
        [this](FloatType x)
        {
            FloatType g = std::tan(x * Tdiv2);
            return g / (FloatType(1.0) + g);
        },
        0,
        OmegaLimit,
        256
    );
}

template class Multimode1 <float>;
template class Multimode1 <double>;

#pragma endregion

#pragma region BallisticsFilter

template<typename FloatType>
void BallisticsFilter<FloatType>::setAttack(FloatType attackMs) noexcept
{
    Ga = mm1.tauToG(attackMs);
}

template<typename FloatType>
void BallisticsFilter<FloatType>::setRelease(FloatType releaseMs) noexcept
{
    Gr = mm1.tauToG(releaseMs);
}

template<typename FloatType>
void BallisticsFilter<FloatType>::reset()
{
    mm1.reset();
}


template<typename FloatType>
void BallisticsFilter<FloatType>::prepare(FloatType sampleRate, size_t numInputChannels)
{
    mm1.prepare(sampleRate, numInputChannels);
}

template class BallisticsFilter<float>;
template class BallisticsFilter<double>;

#pragma endregion

#pragma region MonoConverter

template <typename FloatType>
void MonoConverter<FloatType>::prepare(size_t numInputChannels)
{
    numChannels = numInputChannels;
    divNumChannels = FloatType(1.0) / numInputChannels;
}

template class MonoConverter<float>;
template class MonoConverter<double>;

#pragma endregion

#pragma region KFilter

template<typename FloatType>
void KFilter<FloatType>::reset()
{
    std::fill(_x1.begin(), _x1.end(), 0.0);
    std::fill(_x2.begin(), _x2.end(), 0.0);
    std::fill(_y1.begin(), _y1.end(), 0.0);
    std::fill(_y2.begin(), _y2.end(), 0.0);
}

template<typename FloatType>
void KFilter<FloatType>::prepare(FloatType sampleRate, size_t numInputChannels)
{
    //Parameters
    FloatType Vh = FloatType(1.58), Vb = FloatType(1.26), Vl = FloatType(1);
    FloatType Q = FloatType(0.71);
    FloatType fc = FloatType(1681.97);

    //Prewarped angular cutoff
    FloatType g = tan(VA::MathConstants<FloatType>::pi * fc / sampleRate);
    FloatType gSq = g * g;

    //Calculate Coefficients
    a1 = 2 * (gSq - 1);
    a2 = gSq - g / Q + 1;
    b0 = Vl * gSq + Vb * g / Q + Vh;
    b1 = 2 * (Vl * gSq - Vh);
    b2 = Vl * gSq - Vb * g / Q + Vh;

    //Normalize to a0
    FloatType a0 = gSq + g / Q + 1;
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

template<typename FloatType>
void Detector<FloatType>::reset()
{
    kFilter.reset();
}

template<typename FloatType>
void Detector<FloatType>::prepare(FloatType sampleRate, size_t samplesPerBlock, size_t numInputChannels)
{
    kFilter.prepare(sampleRate, numInputChannels);
}

template class Detector<float>;
template class Detector<double>;

#pragma endregion

} // namespace VA