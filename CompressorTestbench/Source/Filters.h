/*
==============================================================================
  Linear, piecewise linear, and mostly linear filters.

  Zhe Deng 2021
  thezhefromcenterville@gmail.com

  This file is part of CompressorTestBench which is released under the MIT license.
  See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
==============================================================================
*/

#pragma once

#include "Core.h"

namespace VA
{

#pragma region Multimode1

enum class Multimode1FilterType
{
    Lowpass = 1,
    Highpass
};

/** First-order Topology Preserving Transform multimode filter (MM1) */
template <typename SampleType>
class Multimode1
{
public:

    using FilterType = Multimode1FilterType;

    /// <summary>
    /// Set the filter type
    /// </summary>
    /// <param name="type">Choices include high-pass and low-pass</param>
    void setFilterType(FilterType type) noexcept
    {
        filterType = type;
    }

    /// <summary>
    /// Set the angular cutoff frequency (Omega)
    /// </summary>
    /// <remarks>
    /// This function is for programmatically modulating cutoff at audio rates
    /// </remarks>
    /// <param name="Omega">Omega should be in the range [0, Nyquist)</param>
    void setOmega(SampleType Omega) noexcept
    {
        auto g = std::tan(Omega * Tdiv2);
        G = g / (SampleType(1.0) + g);
    }

    /** Set the filter cutoff frequency in hertz
    *
    *   Note: This method doesn't perform checks. Make sure cutoff is below Nyquist before calling.
    */
    void setCutoff(SampleType cutoffHz) noexcept;

    /** Set the time in miliseconds for the step response to reach 1-1/e */
    void setTau(SampleType tauMs) noexcept;

    /** Convert tau in miliseconds to G
    *
    *   Note 1: G is the total integrator gain after prewarping and resolving unit delays.
    *
    *   Note 2: This method doesn't perform checks. Make sure cutoff is below Nyquist before calling.
    */
    SampleType tauToG(SampleType tauMs) noexcept;

    /** Set the internal integrator input gain
    *
    *   Note 1: G is the total integrator input gain after prewarping and resolving unit delays.
    *
    *   Note 2: This method doesn't perform checks. Make sure cutoff is below Nyquist before calling.
    */
    void setG(SampleType newG) noexcept
    {
        G = newG;
    }

    /** Reset the internal state */
    void reset();

    /** Prepare the processing specifications */
    void prepare(SampleType sampleRate, size_t numInputChannels);

    /** Process a sample given a channel */
    SampleType processSample(SampleType x, size_t channel) noexcept
    {
        SampleType s = I1.getState(channel);

        //filter
        SampleType v = (x - s) * G;
        SampleType y = I1.processSample(v, channel);

        //filter type
        return filterType == FilterType::Lowpass ? y : x - y;
    }


private:

    //parameters
    FilterType filterType = FilterType::Lowpass;
    std::atomic<SampleType> G{ SampleType(1.0) };

    //filter
    Integrator<SampleType> I1;

    //spec
    SampleType Tdiv2{ 0.5 };

};

#pragma endregion

#pragma region BallisticsFilter

/** Ballistics Filter based on Multimode1*/
template <typename SampleType>
class BallisticsFilter
{
public:

    /** Set the time in miliseconds for the step response to reach 1-1/e */
    void setAttack(SampleType attackMs) noexcept;

    /** Set the time in miliseconds for inversed step response to reach 1/e */
    void setRelease(SampleType releaseMs) noexcept;

    /** Set the processing specifications */
    void prepare(SampleType sampleRate, size_t numInputChannels);

    /** Reset the internal state */
    void reset();

    SampleType processSample(SampleType x, size_t channel) noexcept
    {
        //branching cutoff
        mm1.setG(x < y ? Gr : Ga);

        //filter
        y = mm1.processSample(x, channel);
        return y;
    }

private:

    //parameters
    SampleType Gr = SampleType(0.5), Ga = SampleType(0.5);

    //filter
    Multimode1<SampleType> mm1;

    //state
    SampleType y{ 0 };
};

#pragma endregion


//#pragma region State Variable Filter
//
//enum class StateVariableFilterType
//{
//    Lowpass = 1,
//    Bandpass,
//    Highpass
//};
//
///** Second-order Topology Preserving Transform state variable filter (SVF) */
//template <typename SampleType>
//class StateVariableFilter
//{
//public:
//
//    using FilterType = StateVariableFilterType;
//
//    /** Set mode to lowpass or highpass */
//    void setFilterType(FilterType type) noexcept
//    {
//        filterType = type;
//    }
//
//    /** Set the angular cutoff frequency
//    *
//    *   Note 1: This function is for programmatically modulating cutoff at audio rates.
//    *
//    *   Note 2: Do not use negative omega values.
//    */
//    void setOmega(SampleType omega) noexcept
//    {
//    }
//
//    /** Set the filter cutoff frequency in hertz
//    *
//    *   Note: This method doesn't perform checks. Make sure cutoff is below Nyquist before calling.
//    */
//    void setCutoff(SampleType cutoffHz) noexcept;
//
//    /** Set the quality factor */
//    void setQuality(SampleType qualityQ) noexcept;
//
//    /** Reset the internal state */
//    void reset();
//
//    /** Prepare the processing specifications */
//    void prepare(SampleType sampleRate, size_t numInputChannels);
//
//    /** Process a sample given a channel */
//    SampleType processSample(SampleType x, size_t channel) noexcept
//    {
//        SampleType& s1 = _s1[channel];
//        SampleType& s2 = _s2[channel];
//
//        //filter
//
//
//
//        //filter type
//        switch (filter)
//        {
//        case FilterType::Lowpass:
//            
//            
//            
//            break;
//        case FilterType::Bandpass:
//            break;
//        default: //FilterType::Highpass:
//            SampleType yHP = (x - g1 * s1 - s2) * d;
//
//            SampleType v1 = g * yHP;
//            s1 += SampleType(2.0) * v1;
//
//            SampleType v2 = g * y;
//        }
//    }
//
//
//private:
//
//    //parameters
//    std::atomic<FilterType> filterType = FilterType::Lowpass;
//    SampleType g1, d, g;
//    SampleType twoG;
//
//    //state
//    std::vector<SampleType> _s1{ 2 };
//    std::vector<SampleType> _s2{ 2 };
//
//    //spec
//    SampleType Tdiv2{ 0.5 };
//
//};
//
//#pragma endregion

#pragma region MonoConverter

/** Multi-channel to mono converter */
template<typename SampleType>
class MonoConverter
{
public:
    /** Prepare the processing specifications */
    void prepare(size_t numInputChannels);

    /** Process a frame given a buffer */
    SampleType processFrame(SampleType** buffer, size_t frame) noexcept
    {
        SampleType y = buffer[0][frame];

        //sum channels
        for (size_t ch = 1; ch < numChannels; ++ch)
            y += buffer[ch][frame];
        y *= divNumChannels;
        return y;
    }

    /** Process a frame */
    SampleType processFrame(std::vector <SampleType>& frame) noexcept
    {
        SampleType y = frame[0];

        //sum channels
        for (size_t ch = 1; ch < numChannels; ++ch)
            y += frame[ch];
        y *= divNumChannels;
        return y;
    }

private:

    //spec
    size_t numChannels{ 1 };
    SampleType divNumChannels{ 1 };
};

#pragma endregion

#pragma region KFilter

/** K-weighting filter for abritrary sample rates
*
*   https://www.eecs.qmul.ac.uk/~josh/documents/2012/MansbridgeFinnReiss-AES1322012-AutoMultitrackFaders.pdf
*/
template <typename SampleType>
class KFilter
{
public:

    /** Reset the internal state */
    void reset();

    /** Prepare the processing specifications */
    void prepare(SampleType sampleRate, size_t numInputChannels);

    /** Process a sample in the specified channel */
    SampleType processSample(SampleType x, size_t channel) noexcept
    {
        auto& x1 = _x1[channel];
        auto& x2 = _x2[channel];
        auto& y1 = _y1[channel];
        auto& y2 = _y2[channel];

        //filter
        //auto y = b0 * x + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
        auto y = std::fma(b0, x, std::fma(b1, x1, std::fma(b2, x2, std::fma(-a1, y1, -a2 * y2))));

        //update state
        x2 = x1;
        x1 = x;
        y2 = y1;
        y1 = y;

        return y;
    }

private:

    //filter coefficients
    SampleType b0{ 0 }, b1{ 0 }, b2{ 0 }, a1{ 0 }, a2{ 0 };

    //state
    std::vector<SampleType> _x1{ 2 }, _x2{ 2 }, _y1{ 2 }, _y2{ 2 };
};

#pragma endregion

#pragma region Detector

enum class DetectorPreFilterType
{
    None = 1,
    KWeighting
};

enum class DetectorRectifierType
{
    Peak = 1,
    HalfWaveRect,
    FullWaveRect
};

template<typename SampleType>
class Detector
{
public:

    /** Set the pre-filter type */
    void setPreFilterType(DetectorPreFilterType type)
    {
        preFilterType = type;
    }

    /** Set the rectifier type */
    void setRectifierType(DetectorRectifierType type)
    {
        rectifierType = type;
    }

    /** Reset the internal state */
    void reset();

    /** Prepare the processing specifications */
    void prepare(SampleType sampleRate, size_t samplesPerBlock, size_t numInputChannels);

    /** Process a sample given a frame and channel */
    SampleType processSample(SampleType x, size_t channel) noexcept
    {
        //Pre-filter and Rectifier
        return processRectifierInternal(processPrefilterInternal(x, channel));
    }

private:

    //helper functions
    SampleType processPrefilterInternal(SampleType x, size_t channel)
    {
        //Detector
        switch (preFilterType)
        {
        case DetectorPreFilterType::None:
            return x;
        default: //DetectorPreFilterType::KWeighting:
            return kFilter.processSample(x, channel);
            break;
        }
    }

    SampleType processRectifierInternal(SampleType x)
    {
        switch (rectifierType)
        {
        case DetectorRectifierType::Peak:
            return std::abs(x);
            break;
        case DetectorRectifierType::HalfWaveRect:
            return (std::exp(x) - SampleType(1.0)) / (std::exp(SampleType(1.0)) - SampleType(1.0));
            break;
        default: //DetectorRectifierType::FullWaveRect:
            return x > 0 ? (std::exp(x) - SampleType(1.0)) / (std::exp(SampleType(1.0)) - SampleType(1.0))
                : (std::exp(-x) - SampleType(1.0)) / (std::exp(SampleType(1.0)) - SampleType(1.0));
            break;
        }
    }

    //parameters
    DetectorPreFilterType preFilterType = DetectorPreFilterType::None;
    DetectorRectifierType rectifierType = DetectorRectifierType::Peak;

    //filter
    KFilter<SampleType> kFilter;
};

#pragma endregion

} // namespace VA

//TODO LUTs and LUT sample count optimization
//TODO detector shockley equation
//TODO solve SVF zero delay eqn using LP or BP as variable, possible optimization? 