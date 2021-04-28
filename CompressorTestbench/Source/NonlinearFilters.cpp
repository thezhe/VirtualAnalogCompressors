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
    omegaLimit = sampleRate * MathConstants<SampleType>::pi * SampleType(0.499);

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

#pragma region NLDET

template<typename SampleType>
void NLDET<SampleType>::reset()
{
    envFast.reset();
    envSlow.reset();
}

template<typename SampleType>
void NLDET<SampleType>::prepare(SampleType sampleRate, size_t numInputChannels)
{
    envFast.prepare(sampleRate, numInputChannels);
    envSlow.prepare(sampleRate, numInputChannels);
}

template class NLDET<float>;
template class NLDET<double>;

#pragma endregion


#pragma region NLMM1_Freq

template<typename SampleType>
void NLMM1_Freq<SampleType>::setLinearCutoff(SampleType cutoffHz) noexcept
{
    auto omegaLin = MathConstants<SampleType>::pi2 * cutoffHz;
    omegaLinSqrt = sqrt(omegaLin);
    auto g = omegaLin * T_2;
    div1plusg = SampleType(1.0) / (SampleType(1.0) + g);
    G = g / (1 + g);
}

template<typename SampleType>
void NLMM1_Freq<SampleType>::setNonlinearity(SampleType nonlinearityN) noexcept
{
    N = nonlinearityN;
}

template<typename SampleType>
void NLMM1_Freq<SampleType>::reset()
{
    std::fill(_s1.begin(), _s1.end(), SampleType(0.0));
}

template<typename SampleType>
void NLMM1_Freq<SampleType>::prepare(SampleType sampleRate, size_t numInputChannels)
{
    T = SampleType(1.0) / sampleRate;
    T_2 = T / SampleType(2.0);

    _s1.resize(numInputChannels);

    reset();
}

template class NLMM1_Freq<float>;
template class NLMM1_Freq<double>;

#pragma endregion

