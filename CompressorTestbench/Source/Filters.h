/*
  ==============================================================================
    Linear filters and their wrappers.
    
    Zhe Deng 2021
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#pragma once
#include "Utility.h"

/** First-order Riemann integrator low-pass
*
*   Note: Use for time domain effects. Implementation is based on 
*   left Riemann sum integrator and unit delay feedback resolution.
*/
template<typename SampleType>
class LP1_Riemann final : public Processor<SampleType>
{
public:

    /** Convert tau in miliseconds to G
    *
    *   Note 1: G is the total integrator gain after prewarping and resolving unit delays.
    *
    *   Note 2: This method doesn't perform checks. Make sure cutoff is below Nyquist before calling.
    */
    xsimd::simd_type<SampleType> tauToG(SampleType tauMs) noexcept;

    /** Set the internal integrator input gain
    *
    *   Note 1: G is the total integrator input gain after prewarping and resolving unit delays.
    *
    *   Note 2: This method doesn't perform checks. Make sure cutoff is below Nyquist before calling.
    */
    void setG(SIMD newG) noexcept 
    { 
        G = newG; 
    }

    /** Set the time in miliseconds for the step response to reach 1-1/e */
    void setTau(SampleType tauMs) noexcept;

    /** Set the angular cutoff frequency
    * 
    *   Note 1: This function is for programmatically modulating cutoff at audio rates. 
    * 
    *   Note 2: This method doesn't perform checks. Make sure cutoff is below Nyquist before calling.
    */
    void setOmega(SIMD omega) noexcept
    {
        G = SIMD(1.0) - xsimd::exp(mT*omega);
    }

    /** Set the filter to operate in RMS mode */
    void setRMS(bool RMSenable)
    {
        RMS = RMSenable;
    }
    
    /** Set the processing specifications */
    void prepare(double sampleRate, int samplesPerBlock, int numInputChannels);

    /** Reset the internal state */
    void reset()
    {
        y = SIMD(0.0);
    }

    /** Process a SIMD sample */
    SIMD processSample(SIMD x) noexcept
    {
        if (RMS)
        {
            SIMD ySq = y * y;
            y = xsimd::sqrt(xsimd::fma(G, (x*x - ySq), ySq)); //y = sqrt(G * (x^2 - y^2) + y^2)
        }
        else
        {
            y = xsimd::fma(G, (x - y), y); //y = G * (x - y) + y
        }
        return y;
    }

private:

    //outputs
    SIMD y;

    //parameter
    SIMD G;
    bool RMS = false;

    //spec
    SampleType mT, mT1000;
};

/** Differential Envelope Technology using Envelope */
//template <typename SampleType>
//class DET : public ProcessorWrapper<SampleType>
//{
//public:
//
//    /** Enable or disable stereo linking
//    *
//    *   Note: Stereo linking sets all detector channels to the average value
//    */
//    void setStereoLink(bool enableStereoLink)
//    {
//        stereoLink = enableStereoLink;
//    }
//
//    /** Set the processing specifications */
//    void prepare(const double sampleRate, const int samplesPerBlock, const int numInputChannels);
//
//    /** Reset the internal state */
//    void reset()
//    {
//        LP1.reset();
//    }
//
//    /** Process a SIMD sample */
//    SIMD processSample(SIMD x) noexcept
//    {
//        //detector
//        detector = xsimd::abs(x);
//        //stereo link
//        if (stereoLink) detector = xsimd::hadd(detector) / SIMD(numChannels);
//        //filter
//        return envFast.processSample(detector) - envSlow.processSample(detector);
//    }
//
//private:
//
//    //filters
//    LP1_Riemann<SampleType> envFast, envSlow;
//
//    //parameters
//    bool stereoLink = false;
//
//    //outputs
//    SIMD detector, transients;
//
//};

/** Ballistics filter using LP1_Riemann */
template <typename SampleType>
class BallisticsFilter final : public Processor<SampleType>
{
public:

    /** Set the time in miliseconds for the step response to reach 1-1/e */
    void setAttack(SampleType attackMs) noexcept;

    /** Set the time in miliseconds for inversed step response to reach 1/e */
    void setRelease(SampleType releaseMs) noexcept;

    /** Set the processing specifications */
    void prepare(double sampleRate, int samplesPerBlock, int numInputChannels);

    void setRMS(bool RMSenable)
    {
        LP1.setRMS(RMSenable);
    }

    /** Reset the internal state */
    void reset()
    {
        LP1.reset();
    }

    /** Process a SIMD sample
    *
    *   Note: Unused channels must be zeroed for stereo link to function
    */
    SIMD processSample(SIMD x) noexcept
    {
        //branching cutoff
        LP1.setG(xsimd::select(x < y, Gr, Ga)); // rect < y ? Ga : Gr 
        //filter
        y = LP1.processSample(x);
        return y;
    }

private:

    //parameters
    SIMD Gr = SIMD(0.5), Ga = SIMD(0.5);
    bool RMS = false;

    //filter
    LP1_Riemann<SampleType> LP1;

    //outputs
    SIMD y;
};

enum class Multimode1FilterType
{
    lowpass,
    highpass
};

/** First-order Topology Preserving Transform multimode filter
*   
*   Note: Use for frequency domain effects. Implementation is based on
*   trapezoidal integration and zero-delay feedback.
*/
template <typename SampleType>
class Multimode1_TPT final : public Processor<SampleType>
{
public:

    using FilterType = Multimode1FilterType;

    /** Set mode to lowpass or highpass */
    void setMode(FilterType mode) noexcept { filterType = mode; }

    /** Set the filter cutoff frequency in hertz 
    *
    *   Note: This method doesn't perform checks. Make sure cutoff is below Nyquist before calling.
    */
    void setCutoff(SampleType cutoffHz) noexcept;

    /** Set the angular cutoff frequency
    *
    *   Note 1: This function is for programmatically modulating cutoff at audio rates.
    *
    *   Note 2: This method doesn't perform checks. Make sure cutoff is below Nyquist before calling.
    */
    void setOmega(SIMD omega) noexcept
    {
        SIMD g = xsimd::tan(omega * T_2);
        G = g / (SIMD(1.0) + g);
    }
    

    /** Set the processing specifications */
    void prepare(double sampleRate, int samplesPerBlock, int numInputChannels);

    /** Reset the internal state */
    void reset()
    {
        s = SIMD(0.0);
    }

    /** Process a SIMD sample */
    SIMD processSample(SIMD x) noexcept
    {
        //filter
        SIMD v = (x - s) * G;
        y = v + s;
        s = y + v;
        //choose mode
        return (filterType == FilterType::lowpass) ? y : x - y;
    }

private:

    //parameter
    SIMD G;
    FilterType filterType = FilterType::lowpass;

    //state
    SIMD s;

    //outputs
    SIMD y;

    //spec
    SampleType Tpi;
    SIMD T_2;

};

/** K Weighting for abritrary sample rates
*   
*   https://www.eecs.qmul.ac.uk/~josh/documents/2012/MansbridgeFinnReiss-AES1322012-AutoMultitrackFaders.pdf
*/
template <typename SampleType>
class KFilter final: Processor<SampleType>
{
public:
    void prepare(double sampleRate, int samplesPerBlock, int numInputChannels);

    void reset()
    {
        x1 = SIMD(0.0);
        x2 = SIMD(0.0);
        y1 = SIMD(0.0);
        y2 = SIMD(0.0);
    }

    SIMD processSample(SIMD x) noexcept
    {
        //filter
        y = b0 * x + b1 * x1 + b2 * x1 - a1 * y1 - a2 * y2;
        
        //update state
        x2 = x1;
        x1 = x;
        y2 = y1;
        y1 = y;

        return y;
    }
private:

    //filter coefficients
    SIMD b0, b1, b2, a1, a2;

    //state
    SIMD x1, x2, y1, y2;

    //output 
    SIMD y;
};

enum class DetectorType
{
    Peak = 1,
    HalfWaveRect,
    FullWaveRect,
    KWeight
};

template<typename SampleType>
class Detector final : Processor<SampleType>
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

    void setMode(DetectorType mode)
    {
        detectorType = mode;
    }

    void prepare(double sampleRate, int samplesPerBlock, int numInputChannels);

    void reset()
    {
        kFilter.reset();
    }

    SIMD processSample(SIMD x) noexcept
    {
        //detector type
        switch (detectorType)
        {
        case DetectorType::Peak:
            y = xsimd::abs(x);
            break;
        case DetectorType::HalfWaveRect:
            y = xsimd::select(x > SIMD(0.0), x * x, SIMD(0.0));
            break;
        case DetectorType::FullWaveRect:
            y = x * x;
            break;
        default: //DetectorType::KWeight
            y = kFilter.processSample(xsimd::abs(x));
            break;
        }
        //stereo link
         return stereoLink ? xsimd::hadd(y) / numChannels : y;
    }

private:

    //parameters
    DetectorType detectorType = DetectorType::Peak;
    bool stereoLink = false;

    //filter
    KFilter<SampleType> kFilter;

    //output
    SIMD y;

    //spec
    SIMD numChannels;
};

///** Delay line implementing constant integer sample delay */
//template<typename SampleType>
//class DelayLine final: Processor<SampleType>
//{
//public:
//
//    /** Prepare the processing specifications */
//    void prepare(double sampleRate, int samplesPerBlock, double maxDelayMs);
//
//    /** Reset the internal state*/
//    void reset()
//    {
//        for (int i = 0; i < maxDelaySamples; ++i)
//            cBuf[i] = 0.0;
//    }
//    
//    /** Set the delay in samples */
//    void setDelay(size_t delaySamples) noexcept;
//
//    /** Write the current sample */
//    void write(SIMD x) noexcept
//    {
//        if (writePos+=4 >= maxDelaySamples) 
//            writePos = 0;
//        xsimd::store_aligned(&cBuf[writePos], x);
//    }
//
//    /** Read the delayed sample */
//    SIMD read() noexcept
//    {
//        if ((readPos+=4) >= maxDelaySamples) 
//            readPos = 0;
//        return xsimd::load_aligned(&cBuf[readPos]);
//    }
//
//private:
//    
//    //spec
//    size_t maxDelaySamples;
//
//    //state
//    size_t writePos, readPos;
//
//    //circular buffer
//    std::unique_ptr<SampleType[]> cBuf;
//};
//
//template<typename SampleType>
//class RMSDetector final: public Processor<SampleType>
//{
//public:
//
//    /** Prepare the processing specifications */
//    void prepare(double sampleRate, int samplesPerBlock, double maxDelayMs);
//
//    /** Reset the internal state*/
//    void reset()
//    {
//        delayLine.reset();
//    }
//
//    /** Set the approximate delay in miliseconds
//    *
//    *   Note 1: Delay time is not exact. Time is rounded
//    *   and may update during write or read.
//    *
//    *   Note 2: Updating delay resets the internal state.
//    */
//    void setWindowSize(double windowSizeMs) noexcept;
//
//    SIMD processSample(SIMD x) noexcept
//    {
//        //single sample window is the same as peak detector
//        if (windowSize == 0) 
//            return xsimd::abs(x);
//        //update sum of squares
//        delayLine.write(x*x);
//        sumSquares += x*x;
//        sumSquares -= delayLine.read();
//        //root of mean
//        return xsimd::sqrt(sumSquares / windowSamples);
//    }
//
//private:
//
//    //parameters
//    SIMD windowSamples;
//    size_t windowSize;
//
//    //state
//    SIMD sumSquares;
//   
//    //DelayLine
//    DelayLine<SampleType> delayLine;
//
//    //spec
//    SampleType sRate;
//};



//TODO SIMD mask MM1 branch
//TODO FMA
//TODO MM1_Riemann omegaToG lookuptable
//TODO rms, lufs detector or any other metrics
//TODO delay line optimization https://abhinavag.medium.com/a-fast-circular-ring-buffer-4d102ef4d4a3
//TODO LUFS delay at least 0.3 sec
//TODO detector shockley equation
