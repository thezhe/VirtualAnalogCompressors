/*
  ==============================================================================
    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#include "Compressors.h"

#pragma region Compressor

template<typename SampleType>
void Compressor<SampleType>::setThreshold(SampleType thrdB) noexcept { thrlin = SIMD(juce::Decibels::decibelsToGain(thrdB)); }

template<typename SampleType>
void Compressor<SampleType>::setRatio(SampleType ratioR) noexcept { exponent = SIMD((1.0 / ratioR) - 1.0); }

template<typename SampleType>
void Compressor<SampleType>::setWet(SampleType wetdB) noexcept { wetLin = SIMD(juce::Decibels::decibelsToGain(wetdB)); }

template<typename SampleType>
void Compressor<SampleType>::setDry(SampleType drydB) noexcept { dryLin = SIMD(juce::Decibels::decibelsToGain(drydB)); }

template class Compressor<float>;
template class Compressor<double>;

#pragma endregion

#pragma region Compressor_IIR

template<typename SampleType>
void Compressor_IIR<SampleType>::prepare(const double sampleRate, const int samplesPerBlock)
{
    ballisticsFilter_IIR.prepare(sampleRate, samplesPerBlock);
    blockSize = samplesPerBlock*SIMD::size;
}

template<typename SampleType>
void Compressor_IIR<SampleType>::setAttack(SampleType attackMs) noexcept { ballisticsFilter_IIR.setAttack(attackMs); }

template<typename SampleType>
void Compressor_IIR<SampleType>::setRelease(SampleType releaseMs) noexcept { ballisticsFilter_IIR.setRelease(releaseMs); }

template class Compressor_IIR<float>;
template class Compressor_IIR<double>;

#pragma endregion

#pragma region Compressor_TPTz

template<typename SampleType>
void Compressor_TPTz<SampleType>::prepare(const double sampleRate, const int samplesPerBlock) 
{ 
    ballisticsFilter_TPTz.prepare(sampleRate, samplesPerBlock);
    blockSize = samplesPerBlock*SIMD::size; 
}

template<typename SampleType>
void Compressor_TPTz<SampleType>::setAttack(SampleType attackMs) noexcept { ballisticsFilter_TPTz.setAttack(attackMs); }

template<typename SampleType>
void Compressor_TPTz<SampleType>::setRelease(SampleType releaseMs) noexcept { ballisticsFilter_TPTz.setRelease(releaseMs); }

template class Compressor_TPTz<float>;
template class Compressor_TPTz<double>;

#pragma endregion

#pragma region Compressor_TPT

template<typename SampleType>
void Compressor_TPT<SampleType>::prepare(const double sampleRate, const int samplesPerBlock) 
{ 
    ballisticsFilter_TPT.prepare(sampleRate, samplesPerBlock);
    blockSize = samplesPerBlock * SIMD::size;
}

template<typename SampleType>
void Compressor_TPT<SampleType>::setAttack(SampleType attackMs) noexcept { ballisticsFilter_TPT.setAttack(attackMs); }

template<typename SampleType>
void Compressor_TPT<SampleType>::setRelease(SampleType releaseMs) noexcept { ballisticsFilter_TPT.setRelease(releaseMs); }

template class Compressor_TPT<float>;
template class Compressor_TPT<double>;

#pragma endregion

#pragma region FFVCA_IIR

template class FFVCA_IIR<float>;
template class FFVCA_IIR<double>;

#pragma endregion

#pragma region FFVCA_TPTz

template class FFVCA_TPTz<float>;
template class FFVCA_TPTz<double>;

#pragma endregion

#pragma region FFVCA_TPT

template class FFVCA_TPT<float>;
template class FFVCA_TPT<double>;

#pragma endregion

#pragma region FBVCA_IIR

template class FBVCA_IIR<float>;
template class FBVCA_IIR<double>;

#pragma endregion

#pragma region FBVCA_TPTz

template class FBVCA_TPTz<float>;
template class FBVCA_TPTz<double>;

#pragma endregion

#pragma region FFVCA_RL_Modulating_TPTz

template<typename SampleType>
void FFVCA_RL_Modulating_TPTz<SampleType>::prepare(const double sampleRate, const int samplesPerBlock)
{
    blockSize = samplesPerBlock * SIMD::size;
    ballisticsFilter.prepare(sampleRate, samplesPerBlock);
    rl.prepare(sampleRate, samplesPerBlock);
}

template<typename SampleType>
void FFVCA_RL_Modulating_TPTz<SampleType>::setAttack(SampleType attackMs) noexcept
{
    ballisticsFilter.setAttack(attackMs);
}

template<typename SampleType>
void FFVCA_RL_Modulating_TPTz<SampleType>::setRelease(SampleType releaseMs) noexcept
{
    ballisticsFilter.setRelease(releaseMs);
}

template<typename SampleType>
void FFVCA_RL_Modulating_TPTz<SampleType>::setLinearCutoffRL(SampleType cutoffHz) noexcept
{
    rl.setLinearCutoff(cutoffHz);
}

template<typename SampleType>
void FFVCA_RL_Modulating_TPTz<SampleType>::setSaturationRL(SampleType saturationConstant) noexcept
{
    rl.setSaturation(saturationConstant);
}

template<typename SampleType>
void FFVCA_RL_Modulating_TPTz<SampleType>::setIntensityRL(SampleType intensitydB) noexcept
{
    rl.setIntensity(intensitydB);
}

template class FFVCA_RL_Modulating_TPTz<float>;
template class FFVCA_RL_Modulating_TPTz<double>;
#pragma endregion
