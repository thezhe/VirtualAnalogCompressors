/*
  ==============================================================================
    A hybrid Transient Designer-Compressor dynamics processor with saturation.
    
    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#pragma once

#include "NonlinearFilters.h"

#ifdef DEBUG

enum class DynamicsProcessorOutputType
{
    Detector = 1,
    EnvelopeFilter,
    TransferFunction,
    Normal
};

#endif 



enum class DynamicsProcessorInputFilterType
{
    None = 1,
    LP1,
    HP1
};


enum class DynamicsProcessorSidechainInputType
{
    Feedforward = 1,
    Feedback,
    External
};

/// <summary>
/// A general <c>DynamicsProcessor</c> with compressor and transient designer capabilities
/// </summary>
/// <typeparam name="SampleType"></typeparam>
template <typename SampleType>
class DynamicsProcessor
{
public:

    using FilterType = Multimode1FilterType;

    using SidechainInputType = DynamicsProcessorSidechainInputType;

    /** Set the sidechain input */
    void setSidechainInputType(SidechainInputType type) noexcept
    {
        sidechainInputType = type;
    }

#ifdef DEBUG

    void setOutputType(DynamicsProcessorOutputType type) noexcept
    {
        outputType = type;
    }

#endif


    /** Enable or disable stereo linking in the detector output
    *
    *   Note: Stereo linking applies the same processing to all channels
    */
    void setStereoLink(bool linkEnable) noexcept;

    /** Set the detector's pre-filter type */
    void setPreFilterType(DetectorPreFilterType type) noexcept;

    /** Set the detector's rectifier type */
    void setRectifierType(DetectorRectifierType type) noexcept;

    /** Set the gain of the detector output in decibels */
    void setDetectorGain(SampleType dB) noexcept;

    /** Set the threshold in decibels */
    void setThreshold(SampleType thrdB) noexcept;

    void setKneeSofteness(SampleType softnessS) noexcept {}

    /** Set the attack time in miliseconds */
    void setAttack(SampleType attackMs) noexcept;

    /** Set the nonlinearity of the ballistics filter during attacks */
    void setAttackNonlinearity(SampleType nonlinearityN) noexcept;

    /** Set the release time in miliseconds*/
    void setRelease(SampleType releaseMs) noexcept;

    /** Set the nonlinearity of the ballistics filter during releases */
    void setReleaseNonlinearity(SampleType nonlinearityN) noexcept;
    
    /** Ratio to use when the envelope is positive */
    void setPositiveEnvelopeRatio(SampleType ratioR) noexcept;

    /** Ratio to use when the envelope is negative */
    void setNegativeEnvelopeRatio(SampleType ratioR) noexcept;

    void setSensitivity(SampleType sensitivity) noexcept;

    /** Set the gain of the processed signal in decibels */
    void setWetGain(SampleType wetdB) noexcept;

    /** Set the gain of the unprocessed signal in decibels */
    void setDryGain(SampleType drydB) noexcept;

    /** Reset the internal state */
    void reset();

    /** Prepare the processing specifications */
    void prepare(SampleType sampleRate, size_t samplesPerBlock, size_t numInputChannels);
    
    /** Process a sample given the channel */
    SampleType processSample(SampleType x, size_t channel) noexcept
    {
        //Detector
        SampleType d = detectorGain * detector.processSample(x, channel);

        //Smoothing
        SampleType env = nlEF.processSample(x, channel);

        //Transfer Function
        SampleType tf = MathFunctions<SampleType>::ttf(x, thrLin, exponentP, exponentN);

#ifdef DEBUG

        switch (outputType)
        {
        case DynamicsProcessorOutputType::Detector:
            return d;
            break;
        case DynamicsProcessorOutputType::EnvelopeFilter:
            return env;
            break;
        case DynamicsProcessorOutputType::TransferFunction:
            return tf;
            break;
        default:
            return x * tf;
            break;
        }

#endif
        //Saturation
        return x * tf;
       
    }

    /** Process a buffer */
    void process(SampleType** buffer) noexcept
    {
        //seperate cases for stereoLink, stereoLink feedback, no SL, noSL feedback
        if (stereoLink)
        {
            switch (sidechainInputType)
            {
            case SidechainInputType::Feedforward:
                for (size_t i = 0; i < blockSize; ++i)
                {
                    SampleType x = monoConverter.processFrame(buffer, i);
                    for (size_t ch = 0; ch < numChannels; ++ch)
                        buffer[ch][i] = std::fma(dryLin, buffer[ch][i], wetLin * processSample(x, ch));
                }
                break;
            case SidechainInputType::Feedback:
                for (size_t i = 0; i < blockSize; ++i)
                {
                    SampleType x = monoConverter.processFrame(_y);
                    for (size_t ch = 0; ch < numChannels; ++ch)
                    {
                        _y[ch] = processSample(x, ch);
                        buffer[ch][i] = std::fma(dryLin, buffer[ch][i], wetLin * _y[ch]);
                    }
                }
                break;
            default: //SidechainInputType::External:
                break;
            }
        }
        else //stereoLink == false
        {
            switch (sidechainInputType)
            {
            case SidechainInputType::Feedforward:
                for (size_t i = 0; i < blockSize; ++i)
                {
                    for (size_t ch = 0; ch < numChannels; ++ch)
                        buffer[ch][i] = std::fma(dryLin, buffer[ch][i], wetLin * processSample(buffer[ch][i], ch));
                }
                break;
            case SidechainInputType::Feedback:
                for (size_t i = 0; i < blockSize; ++i)
                {
                    SampleType x = monoConverter.processFrame(_y);
                    for (size_t ch = 0; ch < numChannels; ++ch)
                    {
                        _y[ch] = processSample(_y[ch], ch);
                        buffer[ch][i] = std::fma(dryLin, buffer[ch][i], wetLin * _y[ch]);
                    }
                }
                break;
            default: //SidechainInputType::External:
                break;
            }
        }
    }

private:
    
    //parameters
    SidechainInputType sidechainInputType = SidechainInputType::Feedforward;
    std::atomic<SampleType> detectorGain = SampleType(1.0);
    std::atomic<bool> stereoLink = true;
    std::atomic<SampleType> thrLin = 1.0, exponent = 0.0;
    std::atomic<SampleType> exponentP = SampleType(0.0), exponentN = SampleType(0.0);
    std::atomic<SampleType> dryLin, wetLin = 1.0;

#ifdef DEBUG

    std::atomic<DynamicsProcessorOutputType> outputType = DynamicsProcessorOutputType::Normal;

#endif


    //filters
    MonoConverter<SampleType> monoConverter;
    Detector<SampleType> detector;
    NLEnvelopeFilter<SampleType> nlEF;

    //state
    std::vector<SampleType> _y{ 2 };   

    //spec
    size_t blockSize, numChannels;
};

//TODO sidechain support
//TODO expansion (upwards and downwards) and compression (upwards and downwards)
//TODO soft knee
//TODO waveshaping transfer functions
//TODO nonlinearityN normalize to [0,1]