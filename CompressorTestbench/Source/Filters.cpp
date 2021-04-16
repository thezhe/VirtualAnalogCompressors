/*
  ==============================================================================
    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#include "Filters.h"

#pragma region LP1_Riemann

template<typename SampleType>
xsimd::simd_type<SampleType> LP1_Riemann<SampleType>::tauToG(SampleType tauMs) noexcept
{
    return SIMD(1.0 - exp(mT1000 / tauMs)); //1-e^(1 / (sampleRate * tau)) = 1-e^(T*1000/tauMs) 
}

template<typename SampleType>
void LP1_Riemann<SampleType>::setTau(SampleType tauMs) noexcept
{
    setG(tauToG(tauMs));
}

template<typename SampleType>
void LP1_Riemann<SampleType>::prepare(double sampleRate, int samplesPerBlock, int numInputChannels)
{
    reset();

    mT = -1.0 / sampleRate;
    mT1000 = -1000.0 / sampleRate;
}

// "How can I avoid linker errors with my template classes?" 
//https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
template class LP1_Riemann <float>;
template class LP1_Riemann <double>;

#pragma endregion

//#pragma region DET
//
//template<typename SampleType>
//void DET<SampleType>::prepare(const double sampleRate, const int samplesPerBlock, const int numInputChannels)
//{
//
//}
//
//template class DET<float>;
//template class DET<double>;
//
//#pragma endregion

#pragma region BallisticsFilter

template<typename SampleType>
void BallisticsFilter<SampleType>::prepare(double sampleRate, int samplesPerBlock, int numInputChannels)
{
    LP1.prepare(sampleRate, samplesPerBlock, numInputChannels);
}

template<typename SampleType>
void BallisticsFilter<SampleType>::setAttack(SampleType attackMs) noexcept
{
    Ga = LP1.tauToG(attackMs);
}

template<typename SampleType>
void BallisticsFilter<SampleType>::setRelease(SampleType releaseMs) noexcept
{
    Gr = LP1.tauToG(releaseMs);
}

template class BallisticsFilter<float>;
template class BallisticsFilter<double>;

#pragma endregion

#pragma region Multimode1_TPT

template<typename SampleType>
void Multimode1_TPT<SampleType>::prepare(double sampleRate, int samplesPerBlock, int numInputChannels)
{
    reset();

    Tpi = MathConstants<SampleType>::pi / sampleRate;
    T_2 = SIMD(0.5 / sampleRate);
}

template<typename SampleType>
void Multimode1_TPT<SampleType>::setCutoff(SampleType cutoffHz) noexcept
{
    SampleType g = tan(Tpi * cutoffHz);
    G = SIMD(g / (1.0 + g));
}

template class Multimode1_TPT <float>;
template class Multimode1_TPT <double>;

#pragma endregion


#pragma region Detector

template<typename SampleType>
void Detector<SampleType>::prepare(double sampleRate, int samplesPerBlock, int numInputChannels)
{
    kFilter.prepare(sampleRate, samplesPerBlock, numInputChannels);
    numChannels = SIMD(numInputChannels);
}

template class Detector<float>;
template class Detector<double>;

#pragma endregion

#pragma region KFilter

template<typename SampleType>
void KFilter<SampleType>::prepare(double sampleRate, int samplesPerBlock, int numInputChannels)
{
    //reset state
    reset();

    //Parameters
    SampleType Vh = 1.58, Vb = 1.26, Vl = 1;
    SampleType Q = 0.71;
    SampleType fc = 1681.97;
    
    //Prewarped angular cutoff
    SampleType g = tan(MathConstants<SampleType>::pi * fc / sampleRate);
    SampleType gSq = g * g;

    //Calculate Coefficients
    a1 = SIMD(2 * (gSq - 1));
    a2 = SIMD(gSq - g / Q + 1);
    b0 = SIMD(Vl * gSq + Vb * g / Q + Vh);
    b1 = SIMD(2 * (Vl * gSq - Vh));
    b2 = SIMD(Vl * gSq - Vb * g / Q + Vh);

    //Normalize to a0
    SIMD a0 = SIMD(gSq + g / Q + 1);
    a1 /= a0;
    a2 /= a0;
    b0 /= a0;
    b1 /= a0;
    b2 /= a0;
}

template class KFilter<float>;
template class KFilter<double>;

#pragma endregion




//#pragma region DelayLine
//
//template<typename SampleType>
//void DelayLine<SampleType>::prepare(double sampleRate, int samplesPerBlock, double maxDelayMs)
//{
//    maxDelaySamples = (1.0 + (maxDelayMs / 1000.0)) * 4.0 * sampleRate;
//    cBuf.reset(new SampleType[maxDelaySamples]);
//    reset();
//}
//
//template<typename SampleType>
//void DelayLine<SampleType>::setDelay(size_t delaySamples) noexcept
//{
//    readPos = (writePos + maxDelaySamples - SIMD::size * delaySamples) % maxDelaySamples;
//}
//
//template class DelayLine<float>;
//template class DelayLine<double>;
//
//#pragma endregion
//
//#pragma region RMSDetector
//
//template<typename SampleType>
//void RMSDetector<SampleType>::prepare(double sampleRate, int samplesPerBlock, double maxDelayMs)
//{
//    delayLine.prepare(sampleRate, samplesPerBlock, maxDelayMs);
//    sRate = sampleRate;
//}
//
//template<typename SampleType>
//void RMSDetector<SampleType>::setWindowSize(double windowSizeMs) noexcept
//{
//    size_t delaySamples = (windowSizeMs / 1000.0) * sRate;
//    delayLine.setDelay(delaySamples);
//    sumSquares = SIMD(0.0);
//    windowSamples = SIMD(delaySamples);
//    windowSize = delaySamples;
//}
//
//template class RMSDetector<float>;
//template class RMSDetector<double>;
//
//#pragma endregion

