/*
  ==============================================================================
    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#include "Dynamics.h"

#pragma region Compressor

template<typename SampleType>
void Compressor<SampleType>::setThreshold(SampleType thrdB) noexcept 
{ 
    thrlin = SIMD(Decibels::decibelsToGain(thrdB)); 
}

template<typename SampleType>
void Compressor<SampleType>::setRatio(SampleType ratioR) noexcept 
{ 
    exponent = SIMD((1.0 / ratioR) - 1.0); 
}

template<typename SampleType>
void Compressor<SampleType>::setInputGain(SampleType inputdB) noexcept
{
    inLin = SIMD(Decibels::decibelsToGain(inputdB));
}

template<typename SampleType>
void Compressor<SampleType>::setWetGain(SampleType wetdB) noexcept 
{ 
    wetLin = SIMD(Decibels::decibelsToGain(wetdB)); 
}

template<typename SampleType>
void Compressor<SampleType>::setDryGain(SampleType drydB) noexcept 
{ 
    dryLin = SIMD(Decibels::decibelsToGain(drydB)); 
}

template<typename SampleType>
void Compressor<SampleType>::setAttack(SampleType attackMs) noexcept
{
    ballisticsFilter.setAttack(attackMs);
}

template<typename SampleType>
void Compressor<SampleType>::setRelease(SampleType releaseMs) noexcept
{
    ballisticsFilter.setRelease(releaseMs);
}

template<typename SampleType>
void Compressor<SampleType>::setLinearTauRL(SampleType linearTauMs) noexcept
{
    frohlichRL.setLinearTau(linearTauMs);
}

template<typename SampleType>
void Compressor<SampleType>::setSaturationRL(SampleType saturationConstant) noexcept
{
    frohlichRL.setSaturation(saturationConstant);
}

template<typename SampleType>
void Compressor<SampleType>::prepare(double sampleRate, int samplesPerBlock, int numInputChannels)
{
    interleavedSize = SIMD::size * samplesPerBlock;

    detector.prepare(sampleRate, samplesPerBlock, numInputChannels);
    ballisticsFilter.prepare(sampleRate, samplesPerBlock, numInputChannels);
    frohlichRL.prepare(sampleRate, samplesPerBlock, numInputChannels);
}

template class Compressor<float>;
template class Compressor<double>;

#pragma endregion

//#pragma region Transient Designer
//
//template<typename SampleType>
//void TransientDesigner<SampleType>::setInputGain(SampleType inputdB) noexcept
//{
//    inLin = SIMD(Decibels::decibelsToGain(inputdB));
//}
//
//template<typename SampleType>
//void TransientDesigner<SampleType>::setWetGain(SampleType wetdB) noexcept
//{
//    wetLin = SIMD(Decibels::decibelsToGain(wetdB));
//}
//
//template<typename SampleType>
//void TransientDesigner<SampleType>::setDryGain(SampleType drydB) noexcept
//{
//    dryLin = SIMD(Decibels::decibelsToGain(drydB));
//}
//
//template<typename SampleType>
//void TransientDesigner<SampleType>::prepare(double sampleRate, int samplesPerBlock, int numInputChannels)
//{
//    Processor<SampleType>::prepare(double sampleRate, int samplesPerBlock, int numInputChannels);
//
//    det.prepare(sampleRate, samplesPerBlock, numInputChannels);
//}
//
//template class TransientDesigner<float>;
//template class TransientDesigner<double>;
//
//#pragma endregion
