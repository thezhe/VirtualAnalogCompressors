/*
  ==============================================================================
    Linear, piecewise linear, and approximately linear filters.
    
    Zhe Deng 2021
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#pragma once
#include "Core.h"

#pragma region Multimode1

enum class Multimode1FilterType
{
    Lowpass = 1,
    Highpass
};

/** First-order Topology Preserving Transform multimode filter */
template <typename SampleType>
class Multimode1
{
public:

    using FilterType = Multimode1FilterType;

    /** Set mode to lowpass or highpass */
    void setFilterType(FilterType type) noexcept
    {
        filterType = type;
    }

    /** Set the angular cutoff frequency
    *
    *   Note 1: This function is for programmatically modulating cutoff at audio rates.
    *
    *   Note 2: Do not use negative omega values.
    */
    void setOmega(SampleType omega) noexcept
    {
        auto g = tan(omega * T_2);
        G =  g / (SampleType(1.0) + g);
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
        auto& s = _s1[channel];

        //filter
        auto v = (x - s) * G;
        auto y = v + s;
        s = y + v;

        //filter type
        return filterType == FilterType::Lowpass ? y : x - y;
    }


private:

    //parameters
    FilterType filterType = FilterType::Lowpass;
    std::atomic<SampleType> G{ SampleType(1.0) };

    //state
    std::vector<SampleType> _s1{ 2 };
    
    //spec
    SampleType T_2{ 0.5 };

};

#pragma endregion

#pragma region MonoConverter

/** Multi-channel to mono converter */
template<typename SampleType>
class MonoConverter
{
public:

    /** Reset the internal state */
    void reset() noexcept
    {
        y = 0;
    }

    /** Prepare the processing specifications */
    void prepare(size_t samplesPerBlock, size_t numInputChannels);

    /** Process a frame given a buffer */
    SampleType processFrame(SampleType** buffer, size_t channel, size_t frame) noexcept
    {
        //reuse mono sample
        if (channel > 0)
            return y;
        
        
        reset();

        //sum channels
        for (size_t ch = 0; ch < numChannels; ++ch)
            y += buffer[ch][frame];
        y *= divNumChannels;
        return y;
    }

    /** Process a frame */
    SampleType processFrame(std::vector <SampleType> frame, size_t channel) noexcept
    {
        //reuse mono sample
        if (channel > 0)
            return y;

        
        reset();

        //sum channels
        for (size_t ch = 0; ch < numChannels; ++ch)
            y += frame[ch];
        y *= divNumChannels;
        return y;
    }

private:

    //state
    SampleType y;

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
    SampleType b0, b1, b2, a1, a2;

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

    /** Enable or disable stereo linking
    *
    *   Note: Stereo linking sets all detector channels to the average value
    */
    void setStereoLink(bool stereoLinkEnable) noexcept
    {
        stereoLink = stereoLinkEnable;
    }

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

    /** Process a sample given a buffer, channel, and frame */
    SampleType processSample(SampleType** buffer, size_t channel, size_t frame) noexcept
    {
        //Stereo Link
        SampleType x = stereoLink ? monoConverter.processFrame(buffer, channel, frame) : buffer[channel][frame];

        //Pre-filter and Rectifier
        return processRectifierInternal(processPrefilterInternal(x, channel));
    }

    /** Process a sample given a frame and channel */
    SampleType processSample(std::vector <SampleType> frame, size_t channel) noexcept
    {
        //Stereo Link
        SampleType x = stereoLink ? monoConverter.processFrame(frame, channel) : frame[channel];

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
            return abs(x);
            break;
        case DetectorRectifierType::HalfWaveRect:
            return (exp(x)-1)/(exp(1)-1);
            break;
        default: //DetectorRectifierType::FullWaveRect:
            return x > 0 ? (exp(x) - 1) / (exp(1) - 1): (exp(-x) - 1) / (exp(1) - 1);
            break;
        }
    }

    //parameters
    DetectorPreFilterType preFilterType = DetectorPreFilterType::None;
    DetectorRectifierType rectifierType = DetectorRectifierType::Peak;
    bool stereoLink = false;

    //filter
    MonoConverter<SampleType> monoConverter;
    KFilter<SampleType> kFilter;
};

#pragma endregion

//TODO LUTs and LUT sample count optimization
//TODO detector shockley equation
