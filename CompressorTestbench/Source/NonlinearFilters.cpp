/*
==============================================================================
Nonlinear filters derived from linear filters.

Zhe Deng 2020
thezhefromcenterville@gmail.com

This file is part of CompressorTestBench which is released under the MIT license.
See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
==============================================================================
*/

#include "NonlinearFilters.h"

namespace VA
{

#pragma region NLMM1_Time

template<typename FloatType>
void NLMM1_Time<FloatType>::setNonlinearity(FloatType nonlinearityN) noexcept
{
    nonlinearity = nonlinearityN;
}

template<typename FloatType>
void NLMM1_Time<FloatType>::setLinearTau(FloatType linearTauMs) noexcept
{
    omegaLinSqrt = sqrt(FloatType(1000.0) / linearTauMs);
}

template<typename FloatType>
void NLMM1_Time<FloatType>::prepare(FloatType sampleRate, size_t numInputChannels)
{
    mm1.prepare(sampleRate, numInputChannels);
    omegaLimit = sampleRate * VA::MathConstants<FloatType>::pi *FloatType(0.499);

    _y.resize(numInputChannels);
    std::fill(_y.begin(), _y.end(), FloatType(0.0));
}

template class NLMM1_Time<float>;
template class NLMM1_Time<double>;

#pragma endregion

#pragma region NLBallisticsFilter

template<typename FloatType>
void NLBallisticsFilter<FloatType>::setAttack(FloatType value) noexcept
{
    aOmegaLinSqrt = std::sqrt(FloatType(1000.0)/value);
}

template<typename FloatType>
void NLBallisticsFilter<FloatType>::setAttackNonlinearity(FloatType value) noexcept
{
    aNonlinearity = value;
}
template<typename FloatType>
void NLBallisticsFilter<FloatType>::setRelease(FloatType value) noexcept
{
    rOmegaLinSqrt = std::sqrt(FloatType(1000.0) / value);
}

template<typename FloatType>
void NLBallisticsFilter<FloatType>::setReleaseNonlinearity(FloatType value) noexcept
{
    rNonlinearity = value;
}

template<typename FloatType>
void NLBallisticsFilter<FloatType>::reset()
{
    nlMM1.reset();
    std::fill(_y.begin(), _y.end(), FloatType(0.0));
}

template<typename FloatType>
void NLBallisticsFilter<FloatType>::prepare(FloatType sampleRate, size_t numInputChannels)
{
    nlMM1.prepare(sampleRate, numInputChannels);
    _y.resize(numInputChannels);

    std::fill(_y.begin(), _y.end(), FloatType(0.0));
}

template class NLBallisticsFilter<float>;
template class NLBallisticsFilter<double>;

#pragma endregion

#pragma region NLEnvelopeFilter

template<typename FloatType>
void NLEnvelopeFilter<FloatType>::setAttack(FloatType attackMs) noexcept
{
    nlbfFast.setAttack(attackMs);
    bfSlow.setAttack(sensitivityRatio * attackMs);
    _attackMs = attackMs;
}

template<typename FloatType>
void NLEnvelopeFilter<FloatType>::setAttackNonlinearity(FloatType nonlinearityN) noexcept
{
    nlbfFast.setAttackNonlinearity(nonlinearityN);
}

template<typename FloatType>
void NLEnvelopeFilter<FloatType>::setRelease(FloatType releaseMs) noexcept
{
    nlbfFast.setRelease(releaseMs);
    bfSlow.setRelease(sensitivityRatio * releaseMs);
    _releaseMs = releaseMs;
}

template<typename FloatType>
void NLEnvelopeFilter<FloatType>::setReleaseNonlinearity(FloatType nonlinearityN) noexcept
{
    nlbfFast.setReleaseNonlinearity(nonlinearityN);
}

template<typename FloatType>
void NLEnvelopeFilter<FloatType>::setSensitivity(FloatType value) noexcept
{
    sensitivityRatio =  value + 1;
    bfSlow.setAttack(sensitivityRatio * _attackMs);
    bfSlow.setRelease(sensitivityRatio * _releaseMs);
}

template<typename FloatType>
void NLEnvelopeFilter<FloatType>::reset()
{
    nlbfFast.reset();
    bfSlow.reset();
}

template<typename FloatType>
void NLEnvelopeFilter<FloatType>::prepare(FloatType sampleRate, size_t numInputChannels)
{
    nlbfFast.prepare(sampleRate, numInputChannels);
    bfSlow.prepare(sampleRate, numInputChannels);
}

template class NLEnvelopeFilter<float>;
template class NLEnvelopeFilter<double>;

#pragma endregion

#pragma region Hysteresis_Time

template<typename FloatType>
void Hysteresis_Time<FloatType>::reset()
{
    D1.reset();
    I1.reset();

    std::fill(_x1.begin(), _x1.end(), FloatType(0.0));
    
}

template<typename FloatType>
void Hysteresis_Time<FloatType>::prepare(FloatType sampleRate, size_t numInputChannels)
{
    Tdiv2 = FloatType(0.5) / sampleRate;
    _x1.resize(numInputChannels);
}

template class Hysteresis_Time<float>;
template class Hysteresis_Time<double>;

#pragma endregion


#pragma region NLMM1_Freq

template<typename FloatType>
void NLMM1_Freq<FloatType>::setLinearCutoff(FloatType cutoffHz) noexcept
{
    FloatType omegaLin = MathFunctions<FloatType>::preWarp(MathConstants<FloatType>::pi2 * cutoffHz, fs2, Tdiv2);
    omegaLinSqrt = std::sqrt(omegaLin);

    FloatType g = omegaLin * Tdiv2;
    div1plusg = FloatType(1.0) / (FloatType(1.0) + g);
    Glin = g / (1 + g);
}

template<typename FloatType>
void NLMM1_Freq<FloatType>::setNonlinearity(FloatType nonlinearityN) noexcept
{
    N = nonlinearityN;
    TN = T * N;
}

template<typename FloatType>
void NLMM1_Freq<FloatType>::reset()
{
    I1.reset();
}

template<typename FloatType>
void NLMM1_Freq<FloatType>::prepare(FloatType sampleRate, size_t numInputChannels, size_t samplesPerBlock)
{
    T = FloatType(1.0) / sampleRate;
    Tdiv2 = FloatType(0.5) / sampleRate;
    fs2 = FloatType(2.0) * sampleRate;

    I1.prepare(numInputChannels);

    blockSize = samplesPerBlock;

    reset();
}

template class NLMM1_Freq<float>;
template class NLMM1_Freq<double>;

#pragma endregion

} // namespace VA