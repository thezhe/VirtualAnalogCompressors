/*
  ==============================================================================
    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of TestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogs for full license details.
  ==============================================================================
*/

#include "Dynamics.h"

#pragma region DynamicsProcessor

template<typename SampleType>
inline void DynamicsProcessor<SampleType>::setStereoLink(bool enable) noexcept
{
    stereoLink = enable;
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setPreFilterType(DetectorPreFilterType type) noexcept
{
    detector.setPreFilterType(type);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setRectifierType(DetectorRectifierType type) noexcept
{
    detector.setRectifierType(type);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setDetectorGain(SampleType gaindB) noexcept
{
    detectorGain = MathFunctions<SampleType>::decibelsToGain(gaindB);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setThreshold(SampleType thrdB) noexcept
{
    thrLin = MathFunctions<SampleType>::decibelsToGain(thrdB);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setAttack(SampleType attackMs) noexcept
{
    nlEF.setAttack(attackMs);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setAttackNonlinearity(SampleType nonlinearityN) noexcept
{
    nlEF.setAttackNonlinearity(nonlinearityN);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setRelease(SampleType releaseMs) noexcept
{
    nlEF.setRelease(releaseMs);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setReleaseNonlinearity(SampleType nonlinearityN) noexcept
{
    nlEF.setReleaseNonlinearity(nonlinearityN);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setPositiveEnvelopeRatio(SampleType ratioR) noexcept 
{ 
    exponentP = SampleType(1.0) / ratioR - SampleType(1.0); 
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setNegativeEnvelopeRatio(SampleType ratioR) noexcept
{
    exponentN = SampleType(1.0) / ratioR - SampleType(1.0);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setSensitivity(SampleType sensitivity) noexcept
{
    nlEF.setSensitivity(sensitivity);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setWetGain(SampleType wetdB) noexcept 
{ 
    wetLin = MathFunctions<SampleType>::decibelsToGain(wetdB); 
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setDryGain(SampleType drydB) noexcept 
{ 
    dryLin = MathFunctions<SampleType>::decibelsToGain(drydB); 
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::reset()
{
    //detector
    detector.reset();

    //filters
    nlEF.reset();

    //state
    std::fill(_y.begin(), _y.end(), SampleType(0.0));
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::prepare(SampleType sampleRate, size_t samplesPerBlock, size_t numInputChannels)
{
    //detector
    detector.prepare(sampleRate, samplesPerBlock, numInputChannels);

    //filters
    nlEF.prepare(sampleRate, numChannels);
    
    //state
    _y.resize(numInputChannels);
    std::fill(_y.begin(), _y.end(), SampleType(0.0));

    //spec
    blockSize = samplesPerBlock;
    numChannels = numInputChannels;
}

template class DynamicsProcessor<float>;
template class DynamicsProcessor<double>;

#pragma endregion
