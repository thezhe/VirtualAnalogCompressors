/*
  ==============================================================================

    VCA Compressor Classes
    Zhe Deng 2021

  ==============================================================================
*/

#include "Compressors.h"

//template<typename SampleType>
void Compressor/*<SampleType>*/::setThreshold(float thrdB) noexcept { thrlin = juce::Decibels::decibelsToGain(thrdB); }

//template<typename SampleType>
void Compressor/*<SampleType>*/::setRatio(float ratioR) noexcept { exponent = (1.0 / ratioR) - 1.0; }

void Compressor::setWet(float wetdB) noexcept { wetLin = juce::Decibels::decibelsToGain(wetdB); }

void Compressor::setDry(float drydB) noexcept { dryLin = juce::Decibels::decibelsToGain(drydB); }

//template<typename SampleType>
void TPTCompressor/*<SampleType>*/::prepare(const double sampleRate, const int samplesPerBlock)
{
    blockSize = samplesPerBlock;
    Tdiv2 = 1.0 / sampleRate;
}

//template<typename SampleType>
void TPTCompressor/*<SampleType>*/::setAttack(float attackMs) noexcept
{ 
    float w = 1000.0 / attackMs;
    float g = tan(Tdiv2*w);
    Ga = g / (1.0 + g);
}

//template<typename SampleType>
void TPTCompressor/*<SampleType>*/::setRelease(float releaseMs) noexcept
{
    float w = 1000.0 / releaseMs;
    float g = tan(Tdiv2 * w);
    Gr = g / (1.0 + g);
}


//template<typename SampleType>
void DECompressor/*<SampleType>*/::prepare(const double sampleRate, const int samplesPerBlock)
{
    blockSize = samplesPerBlock;
    T = 1.0 / sampleRate;
}

//template<typename SampleType>
void DECompressor/*<SampleType>*/::setAttack(float attackMs) noexcept
{
    float w = 1000.0 / attackMs;
    a_a = exp(-w * T);
}

//template<typename SampleType>
void DECompressor/*<SampleType>*/::setRelease(float releaseMs) noexcept
{
    float w = 1000.0 / releaseMs;
    a_r = exp(-w * T);
}
