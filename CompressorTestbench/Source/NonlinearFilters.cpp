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

template<typename SampleType>
void NLMM1_Time<SampleType>::setNonlinearity(SampleType nonlinearityN) noexcept
{
    nonlinearity = nonlinearityN;
}

template<typename SampleType>
void NLMM1_Time<SampleType>::setLinearTau(SampleType linearTauMs) noexcept
{
    omegaLinSqrt = sqrt(SampleType(1000.0) / linearTauMs);
}

template<typename SampleType>
void NLMM1_Time<SampleType>::prepare(SampleType sampleRate, size_t numInputChannels)
{
    mm1.prepare(sampleRate, numInputChannels);
    omegaLimit = sampleRate * VA::MathConstants<SampleType>::pi *SampleType(0.499);

    _y.resize(numInputChannels);
    std::fill(_y.begin(), _y.end(), SampleType(0.0));
}

template class NLMM1_Time<float>;
template class NLMM1_Time<double>;

#pragma endregion

#pragma region NLBallisticsFilter

template<typename SampleType>
void NLBallisticsFilter<SampleType>::setAttack(SampleType attackMs) noexcept
{
    aOmegaLinSqrt = sqrt(MathFunctions<SampleType>::tauToOmega(attackMs));
}

template<typename SampleType>
void NLBallisticsFilter<SampleType>::setAttackNonlinearity(SampleType nonlinearityN) noexcept
{
    aNonlinearity = nonlinearityN;
}
template<typename SampleType>
void NLBallisticsFilter<SampleType>::setRelease(SampleType releaseMs) noexcept
{
    rOmegaLinSqrt = sqrt(MathFunctions<SampleType>::tauToOmega(releaseMs));
}

template<typename SampleType>
void NLBallisticsFilter<SampleType>::setReleaseNonlinearity(SampleType nonlinearityN) noexcept
{
    rNonlinearity = nonlinearityN;
}

template<typename SampleType>
void NLBallisticsFilter<SampleType>::reset()
{
    nlMM1.reset();
    std::fill(_y.begin(), _y.end(), SampleType(0.0));
}

template<typename SampleType>
void NLBallisticsFilter<SampleType>::prepare(SampleType sampleRate, size_t numInputChannels)
{
    nlMM1.prepare(sampleRate, numInputChannels);
    _y.resize(numInputChannels);

    std::fill(_y.begin(), _y.end(), SampleType(0.0));
}

template class NLBallisticsFilter<float>;
template class NLBallisticsFilter<double>;

#pragma endregion

#pragma region NLEnvelopeFilter

template<typename SampleType>
void NLEnvelopeFilter<SampleType>::setAttack(SampleType attackMs) noexcept
{
    nlbfFast.setAttack(attackMs);
    bfSlow.setAttack(sensitivityRatio * attackMs);
    _attackMs = attackMs;
}

template<typename SampleType>
void NLEnvelopeFilter<SampleType>::setAttackNonlinearity(SampleType nonlinearityN) noexcept
{
    nlbfFast.setAttackNonlinearity(nonlinearityN);
}

template<typename SampleType>
void NLEnvelopeFilter<SampleType>::setRelease(SampleType releaseMs) noexcept
{
    nlbfFast.setRelease(releaseMs);
    bfSlow.setRelease(sensitivityRatio * releaseMs);
    _releaseMs = releaseMs;
}

template<typename SampleType>
void NLEnvelopeFilter<SampleType>::setReleaseNonlinearity(SampleType nonlinearityN) noexcept
{
    nlbfFast.setReleaseNonlinearity(nonlinearityN);
}

template<typename SampleType>
void NLEnvelopeFilter<SampleType>::setSensitivity(SampleType sensitivity) noexcept
{
    sensitivityRatio = SampleType(1.0) + sensitivity;
    nlbfFast.setAttack(sensitivityRatio * _attackMs);
    bfSlow.setAttack(sensitivityRatio * _releaseMs);
}

template<typename SampleType>
void NLEnvelopeFilter<SampleType>::reset()
{
    nlbfFast.reset();
    bfSlow.reset();
}

template<typename SampleType>
void NLEnvelopeFilter<SampleType>::prepare(SampleType sampleRate, size_t numInputChannels)
{
    nlbfFast.prepare(sampleRate, numInputChannels);
    bfSlow.prepare(sampleRate, numInputChannels);
}

template class NLEnvelopeFilter<float>;
template class NLEnvelopeFilter<double>;

#pragma endregion

#pragma region Hysteresis_Time

template<typename SampleType>
void Hysteresis_Time<SampleType>::reset()
{
    D1.reset();
    I1.reset();

    std::fill(_x1.begin(), _x1.end(), SampleType(0.0));
    
}

template<typename SampleType>
void Hysteresis_Time<SampleType>::prepare(SampleType sampleRate, size_t numInputChannels)
{
    Tdiv2 = SampleType(0.5) / sampleRate;
    _x1.resize(numInputChannels);
}

template class Hysteresis_Time<float>;
template class Hysteresis_Time<double>;

#pragma endregion


#pragma region NLMM1_Freq

template<typename SampleType>
void NLMM1_Freq<SampleType>::setLinearCutoff(SampleType cutoffHz) noexcept
{
    SampleType omegaLin = MathFunctions<SampleType>::preWarp(MathConstants<SampleType>::pi2 * cutoffHz, fs2, Tdiv2);
    omegaLinSqrt = std::sqrt(omegaLin);

    SampleType g = omegaLin * Tdiv2;
    div1plusg = SampleType(1.0) / (SampleType(1.0) + g);
    Glin = g / (1 + g);
}

template<typename SampleType>
void NLMM1_Freq<SampleType>::setNonlinearity(SampleType nonlinearityN) noexcept
{
    N = nonlinearityN;
    TN = T * N;
}

template<typename SampleType>
void NLMM1_Freq<SampleType>::reset()
{
    I1.reset();
}

template<typename SampleType>
void NLMM1_Freq<SampleType>::prepare(SampleType sampleRate, size_t numInputChannels, size_t samplesPerBlock)
{
    T = SampleType(1.0) / sampleRate;
    Tdiv2 = SampleType(0.5) / sampleRate;
    fs2 = SampleType(2.0) * sampleRate;

    I1.prepare(numInputChannels);

    blockSize = samplesPerBlock;

    reset();
}

template class NLMM1_Freq<float>;
template class NLMM1_Freq<double>;

#pragma endregion

} // namespace VA