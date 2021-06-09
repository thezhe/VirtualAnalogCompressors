/*
==============================================================================
Zhe Deng 2020
thezhefromcenterville@gmail.com

This file is part of TestBench which is released under the MIT license.
See file LICENSE or go to https://github.com/thezhe/VirtualAnalogs for full license details.
==============================================================================
*/

#include "Dynamics.h"
#include "Filters.h"

namespace VA
{

#pragma region DynamicsProcessor

template<typename FloatType>
inline void DynamicsProcessor<FloatType>::setStereoLink(bool enable) noexcept
{
    stereoLink = enable;
}

template<typename FloatType>
void DynamicsProcessor<FloatType>::setPreFilterType(DetectorPreFilterType type) noexcept
{
    detector.setPreFilterType(type);
}

template<typename FloatType>
void DynamicsProcessor<FloatType>::setRectifierType(DetectorRectifierType type) noexcept
{
    detector.setRectifierType(type);
}

template<typename FloatType>
void DynamicsProcessor<FloatType>::setAttack(FloatType attackMs) noexcept
{
    nlEF.setAttack(attackMs);
}

template<typename FloatType>
void DynamicsProcessor<FloatType>::setAttackNonlinearity(FloatType nonlinearityN) noexcept
{
    nlEF.setAttackNonlinearity(nonlinearityN);
}

template<typename FloatType>
void DynamicsProcessor<FloatType>::setRelease(FloatType releaseMs) noexcept
{
    nlEF.setRelease(releaseMs);
}

template<typename FloatType>
void DynamicsProcessor<FloatType>::setReleaseNonlinearity(FloatType nonlinearityN) noexcept
{
    nlEF.setReleaseNonlinearity(nonlinearityN);
}

template<typename FloatType>
void DynamicsProcessor<FloatType>::setSensitivity(FloatType sensitivity) noexcept
{
    nlEF.setSensitivity(sensitivity);
}

template<typename FloatType>
void DynamicsProcessor<FloatType>::setWetGain(FloatType wetdB) noexcept
{
    wetLin = MathFunctions<FloatType>::decibelsToGain(wetdB);
}

template<typename FloatType>
void DynamicsProcessor<FloatType>::setDryGain(FloatType drydB) noexcept
{
    dryLin = MathFunctions<FloatType>::decibelsToGain(drydB);
}

template<typename FloatType>
void DynamicsProcessor<FloatType>::reset()
{
    //detector
    detector.reset();

    //filters
    nlEF.reset();
    hysteresis.reset();

    //state
    std::fill(_y.begin(), _y.end(), FloatType(0.0));
}

template<typename FloatType>
void DynamicsProcessor<FloatType>::prepare(FloatType sampleRate, size_t samplesPerBlock, size_t numInputChannels)
{
    monoConverter.prepare(numInputChannels);

    //detector
    detector.prepare(sampleRate, samplesPerBlock, numInputChannels);

    //filters
    nlEF.prepare(sampleRate, numChannels);
    hysteresis.prepare(sampleRate, numInputChannels);

    //state
    _y.resize(numInputChannels);
    std::fill(_y.begin(), _y.end(), FloatType(0.0));

    //spec
    blockSize = samplesPerBlock;
    numChannels = numInputChannels;


    bf.prepare(sampleRate, numInputChannels);
    bf.setAttack(5);
    bf.setRelease(50);
}

template class DynamicsProcessor<float>;
template class DynamicsProcessor<double>;

#pragma endregion

} // namespace VA