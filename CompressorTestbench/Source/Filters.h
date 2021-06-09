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

/// <summary>
/// First-order multimode filter composed of high-pass and low-pass outputs
/// </summary>
/// <remarks>
/// Implemented with Topology Preserving Transform.
/// Source: https://www.kvraudio.com/forum/viewtopic.php?t=350246
/// </remarks>
template <typename FloatType>
class Multimode1
{
public:

    using FilterType = Multimode1FilterType;

    /// <summary>
    /// Set the filter type
    /// </summary>
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
    /// <param name="Omega">Omega should be in the range [0, inf), although values greater than Nyquist will be mapped to Nyquist first. </param>
    void setOmega(FloatType Omega) noexcept
    {
        G = OmegaToGLUT.processSampleMaxChecked(Omega);
    }

    /// <summary>
    /// Set the cutoff frequency in hertz
    /// </summary>
    void setCutoff(FloatType value) noexcept;

    /// <summary>
    /// Set the time in milliseconds for the step resposne to reach 1-1/e
    /// </summary>
    void setTau(FloatType value) noexcept;

    /// <summary>
    /// Map tau in milliseconds to G
    /// </summary>
    FloatType tauToG(FloatType value) noexcept;

    /// <summary>
    /// Set the integrator input gain <c>G</c>
    /// </summary>
    void setG(FloatType value) noexcept
    {
        G = value;
    }

    /// <summary>
    /// Reset the internal state
    /// </summary>
    void reset();

    /// <summary>
    /// Prepare the processing specifications
    /// </summary>
    void prepare(FloatType sampleRate, size_t numInputChannels);

    /// <summary>
    /// Process a sample given the channel
    /// </summary>
    FloatType processSample(FloatType x, size_t channel) noexcept
    {
        FloatType s = I1.getState(channel);

        //filter
        FloatType v = (x - s) * G;
        FloatType y = I1.processSample(v, channel);

        //filter type
        return filterType == FilterType::Lowpass ? y : x - y;
    }

private:

    //LUT
    static LookupTable<FloatType> OmegaToGLUT;

    //parameters
    FilterType filterType = FilterType::Lowpass;
    std::atomic<FloatType> G{ FloatType(1.0) };

    //filter
    Integrator<FloatType> I1;

    //spec
    FloatType Tdiv2{ 0.5 };

};

//Static object initialization
//https://stackoverflow.com/questions/29552037/c-declaring-a-static-object-in-a-class
template<typename FloatType>
LookupTable<FloatType> Multimode1<FloatType>::OmegaToGLUT;

#pragma endregion

#pragma region BallisticsFilter

/** Ballistics Filter based on Multimode1*/
template <typename FloatType>
class BallisticsFilter
{
public:

    /** Set the time in miliseconds for the step response to reach 1-1/e */
    void setAttack(FloatType attackMs) noexcept;

    /** Set the time in miliseconds for inversed step response to reach 1/e */
    void setRelease(FloatType releaseMs) noexcept;

    /** Set the processing specifications */
    void prepare(FloatType sampleRate, size_t numInputChannels);

    /** Reset the internal state */
    void reset();

    FloatType processSample(FloatType x, size_t channel) noexcept
    {
        //branching cutoff
        mm1.setG(x < y ? Gr : Ga);

        //filter
        y = mm1.processSample(x, channel);
        return y;
    }

private:

    //parameters
    FloatType Gr = FloatType(0.5), Ga = FloatType(0.5);

    //filter
    Multimode1<FloatType> mm1;

    //state
    FloatType y{ 0 };
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
//template <typename FloatType>
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
//    void setOmega(FloatType omega) noexcept
//    {
//    }
//
//    /** Set the filter cutoff frequency in hertz
//    *
//    *   Note: This method doesn't perform checks. Make sure cutoff is below Nyquist before calling.
//    */
//    void setCutoff(FloatType cutoffHz) noexcept;
//
//    /** Set the quality factor */
//    void setQuality(FloatType qualityQ) noexcept;
//
//    /** Reset the internal state */
//    void reset();
//
//    /** Prepare the processing specifications */
//    void prepare(FloatType sampleRate, size_t numInputChannels);
//
//    /** Process a sample given a channel */
//    FloatType processSample(FloatType x, size_t channel) noexcept
//    {
//        FloatType& s1 = _s1[channel];
//        FloatType& s2 = _s2[channel];
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
//            FloatType yHP = (x - g1 * s1 - s2) * d;
//
//            FloatType v1 = g * yHP;
//            s1 += FloatType(2.0) * v1;
//
//            FloatType v2 = g * y;
//        }
//    }
//
//
//private:
//
//    //parameters
//    std::atomic<FilterType> filterType = FilterType::Lowpass;
//    FloatType g1, d, g;
//    FloatType twoG;
//
//    //state
//    std::vector<FloatType> _s1{ 2 };
//    std::vector<FloatType> _s2{ 2 };
//
//    //spec
//    FloatType Tdiv2{ 0.5 };
//
//};
//
//#pragma endregion

#pragma region MonoConverter

/** Multi-channel to mono converter */
template<typename FloatType>
class MonoConverter
{
public:
    /** Prepare the processing specifications */
    void prepare(size_t numInputChannels);

    /** Process a frame given a buffer */
    FloatType processFrame(FloatType** buffer, size_t frame) noexcept
    {
        FloatType y = buffer[0][frame];

        //sum channels
        for (size_t ch = 1; ch < numChannels; ++ch)
            y += buffer[ch][frame];
        y *= divNumChannels;
        return y;
    }

    /** Process a frame */
    FloatType processFrame(std::vector <FloatType>& frame) noexcept
    {
        FloatType y = frame[0];

        //sum channels
        for (size_t ch = 1; ch < numChannels; ++ch)
            y += frame[ch];
        y *= divNumChannels;
        return y;
    }

private:

    //spec
    size_t numChannels{ 1 };
    FloatType divNumChannels{ 1 };
};

#pragma endregion

#pragma region KFilter

/** K-weighting filter for abritrary sample rates
*
*   https://www.eecs.qmul.ac.uk/~josh/documents/2012/MansbridgeFinnReiss-AES1322012-AutoMultitrackFaders.pdf
*/
template <typename FloatType>
class KFilter
{
public:

    /** Reset the internal state */
    void reset();

    /** Prepare the processing specifications */
    void prepare(FloatType sampleRate, size_t numInputChannels);

    /** Process a sample in the specified channel */
    FloatType processSample(FloatType x, size_t channel) noexcept
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
    FloatType b0{ 0 }, b1{ 0 }, b2{ 0 }, a1{ 0 }, a2{ 0 };

    //state
    std::vector<FloatType> _x1{ 2 }, _x2{ 2 }, _y1{ 2 }, _y2{ 2 };
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

template<typename FloatType>
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
    void prepare(FloatType sampleRate, size_t samplesPerBlock, size_t numInputChannels);

    /** Process a sample given a frame and channel */
    FloatType processSample(FloatType x, size_t channel) noexcept
    {
        //Pre-filter and Rectifier
        return processRectifierInternal(processPrefilterInternal(x, channel));
    }

private:

    //helper functions
    FloatType processPrefilterInternal(FloatType x, size_t channel)
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

    FloatType processRectifierInternal(FloatType x)
    {
        switch (rectifierType)
        {
        case DetectorRectifierType::Peak:
            return std::abs(x);
            break;
        case DetectorRectifierType::HalfWaveRect:
            return (std::exp(x) - FloatType(1.0)) / (std::exp(FloatType(1.0)) - FloatType(1.0));
            break;
        default: //DetectorRectifierType::FullWaveRect:
            return x > 0 ? (std::exp(x) - FloatType(1.0)) / (std::exp(FloatType(1.0)) - FloatType(1.0))
                : (std::exp(-x) - FloatType(1.0)) / (std::exp(FloatType(1.0)) - FloatType(1.0));
            break;
        }
    }

    //parameters
    DetectorPreFilterType preFilterType = DetectorPreFilterType::None;
    DetectorRectifierType rectifierType = DetectorRectifierType::Peak;

    //filter
    KFilter<FloatType> kFilter;
};

#pragma endregion

} // namespace VA

//TODO LUTs and LUT sample count optimization
//TODO detector shockley equation
//TODO solve SVF zero delay eqn using LP or BP as variable, possible optimization? 