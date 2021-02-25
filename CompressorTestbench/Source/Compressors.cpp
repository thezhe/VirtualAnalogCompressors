/*
  ==============================================================================

    VCA Compressor Classes
    Zhe Deng 2021

  ==============================================================================
*/

#include "Compressors.h"

void Compressor::setThreshold(float thrdB) noexcept { thrlin = juce::Decibels::decibelsToGain(thrdB); }

void Compressor::setRatio(float ratioR) noexcept { exponent = (1.f / ratioR) - 1.f; }


void TPTCompressor::setAttack(float attackMs) noexcept 
{ 
    float w = 1000.f / attackMs;
    float g = tan(Tdiv2*w);
    Ga = g / (1.f + g);
}

void TPTCompressor::setRelease(float releaseMs) noexcept 
{
    float w = 1000.f / releaseMs;
    float g = tan(Tdiv2 * w);
    Gr = g / (1.f + g);
}

void TPTCompressor::prepare(const double sampleRate, const int samplesPerBlock)
{
    blockSize = samplesPerBlock;
    Tdiv2 = 1.f/sampleRate;
}


void DECompressor::prepare(const double sampleRate, const int samplesPerBlock)
{
    blockSize = samplesPerBlock;
    T = 1.f / sampleRate;
}

void DECompressor::setAttack(float attackMs) noexcept
{
    float w = 1000.f / attackMs;
    a_a = exp(-w * T);
}

void DECompressor::setRelease(float releaseMs) noexcept
{
    float w = 1000.f / releaseMs;
    a_r = exp(-w * T);
}
