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


