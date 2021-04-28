/*
  ==============================================================================
    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#include "Dynamics.h"

#pragma region DynamicsProcessor

template<typename SampleType>
inline void DynamicsProcessor<SampleType>::setStereoLink(bool linkEnable) noexcept
{
    detector.setStereoLink(linkEnable);
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
    detectorGain = Decibels::decibelsToGain(gaindB);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setThreshold(SampleType thrdB) noexcept
{
    thrLin = Decibels::decibelsToGain(thrdB);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setCompressorAttack(SampleType attackMs) noexcept
{
    nlBallisticsFilter.setAttack(attackMs);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setCompressorAttackNonlinearity(SampleType nonlinearityN) noexcept
{
    nlBallisticsFilter.setAttackNonlinearity(nonlinearityN);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setCompressorRelease(SampleType releaseMs) noexcept
{
    nlBallisticsFilter.setRelease(releaseMs);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setCompressorReleaseNonlinearity(SampleType nonlinearityN) noexcept
{
    nlBallisticsFilter.setReleaseNonlinearity(nonlinearityN);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setCompressorRatio(SampleType ratioR) noexcept 
{ 
    exponent = SampleType(1.0) / ratioR - SampleType(1.0); 
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setTransientDesignerTau(SampleType tauMs) noexcept
{
    nlDET.setTau(tauMs);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setTransientDesignerSensitivity(SampleType sensS) noexcept
{
    nlDET.setSensitivity(sensS);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setTransientDesignerNonlinearity(SampleType nonlinearityN) noexcept
{
    nlDET.setNonlinearity(nonlinearityN);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setTransientDesignerAttackRatio(SampleType ratioR) noexcept
{
    exponentA = MathFunctions<SampleType>::ratioToExponent(ratioR);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setTransientDesignerReleaseRatio(SampleType ratioR) noexcept
{
    exponentR = MathFunctions<SampleType>::ratioToExponent(ratioR);
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setWetGain(SampleType wetdB) noexcept 
{ 
    wetLin = Decibels::decibelsToGain(wetdB); 
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::setDryGain(SampleType drydB) noexcept 
{ 
    dryLin = Decibels::decibelsToGain(drydB); 
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::reset()
{
    //detector
    detector.reset();

    //filters
    nlMM1.reset();
    nlBallisticsFilter.reset();
    nlDET.reset();

    //state
    std::fill(_y.begin(), _y.end(), SampleType(0.0));
}

template<typename SampleType>
void DynamicsProcessor<SampleType>::prepare(SampleType sampleRate, size_t samplesPerBlock, size_t numInputChannels)
{
    //detector
    detector.prepare(sampleRate, samplesPerBlock, numInputChannels);

    //filters
    nlMM1.prepare(sampleRate, numInputChannels);
    nlBallisticsFilter.prepare(sampleRate, numInputChannels);
    nlDET.prepare(sampleRate, numInputChannels);
    
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
