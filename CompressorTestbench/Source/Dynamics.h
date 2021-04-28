/*
  ==============================================================================
    A Dynamic Processor composed of a Transient Designer and Compressor.
    
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
    Filter,
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

/** Dynamic range compressor */
template <typename SampleType>
class DynamicsProcessor
{
public:

    using FilterType = DynamicsProcessorInputFilterType;

    using SidechainInputType = DynamicsProcessorSidechainInputType;

    DynamicsProcessor() {}

    void setInputFilterType(FilterType type) noexcept
    {

    }


    void setInputFilterCutoff(SampleType cutoffHz) noexcept
    {

    }

    void setInputFilterFeedbackSaturation(bool feedback) noexcept
    {

    }

    void setInputFilterSaturation(SampleType nonlinearityN) noexcept
    {

    }

    /** Set the sidechain input */
    void setSidechainInputType(SidechainInputType type) noexcept
    {
        sidechainInputType = type;
    }

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

    /** Set the effect as Compressor (and Expander) or Transient Designer */
    void setCompressor(bool enable) noexcept
    {
        compressor = enable;
    }

#pragma region Compressor Only

    /** Set the attack time in miliseconds */
    void setCompressorAttack(SampleType attackMs) noexcept;

    /** Set the nonlinearity of the ballistics filter during attacks */
    void setCompressorAttackNonlinearity(SampleType nonlinearityN) noexcept;

    /** Set the release time in miliseconds*/
    void setCompressorRelease(SampleType releaseMs) noexcept;

    /** Set the nonlinearity of the ballistics filter during releases */
    void setCompressorReleaseNonlinearity(SampleType nonlinearityN) noexcept;
    
    /** Set the ratio */
    void setCompressorRatio(SampleType ratioR) noexcept;

#pragma endregion

#pragma region Transient Designer Only

    /** Set the DET tau in miliseconds */
    void setTransientDesignerTau(SampleType tauMs) noexcept;

    /** Set the DET sensitivity */
    void setTransientDesignerSensitivity(SampleType sensS) noexcept;

    /** Set the DET nonlinearity */
    void setTransientDesignerNonlinearity(SampleType nonlinearityN) noexcept;

    /** Set the DET attack ratio */
    void setTransientDesignerAttackRatio(SampleType ratioR) noexcept;

    /** Set the DET release ratio */
    void setTransientDesignerReleaseRatio(SampleType ratioR) noexcept;

#pragma endregion

    /** Set the gain of the processed signal in decibels */
    void setWetGain(SampleType wetdB) noexcept;

    /** Set the gain of the unprocessed signal in decibels */
    void setDryGain(SampleType drydB) noexcept;


#ifdef DEBUG
    /** Ouput the signal in the selected node of the sidechain 
    *
    *   Note: Debug only
    */
    void setOutputType(DynamicsProcessorOutputType output) noexcept
    {
        outputType = output;
    }
#endif

    /** Reset the internal state */
    void reset();

    /** Prepare the processing specifications */
    void prepare(SampleType sampleRate, size_t samplesPerBlock, size_t numInputChannels);
    
    /** Process a sample given a buffer, channel, and frame */
    SampleType processSample(SampleType** buffer, size_t channel, size_t frame) noexcept
    {
        auto& y = _y[channel];
        auto x = buffer[channel][frame];
        SampleType d;

        //Detector
        if (sidechainInputType == SidechainInputType::Feedforward)
            d = detector.processSample(buffer, channel, frame);
        else
            d = detector.processSample(_y, channel);

        //Detector Gain
        d *= detectorGain;

        SampleType b, tf;
        //Smoothing and Mapping to Vc
        if (compressor)
        {
            //Ballistics Filter
            b = nlBallisticsFilter.processSample(d, channel);

            tf = MathFunctions<SampleType>::ctf(b, thrLin, exponent);

            //ctf        
            y = x * tf;
        }
        else
        {
            //DET
            b = nlDET.processSample(d, channel);

            tf = MathFunctions<SampleType>::ttf(b, thrLin, exponentA, exponentR);
            //ttf        
            y = x * tf;
        }
        
#ifdef DEBUG
        switch (outputType)
        {
        case DynamicsProcessorOutputType::Detector:
            return d;
            break;
        case DynamicsProcessorOutputType::Filter:
            return b;
            break;
        case DynamicsProcessorOutputType::TransferFunction:
            return tf;
            break;
        default: //DynamicsProcessorOutputType::Normal:
            return dryLin * x + wetLin * y;
            break;
        }
#endif

        //mix
        return dryLin * x + wetLin * y;
    }

    /** Process a buffer */
    void process(SampleType** buffer) noexcept
    {
        for (size_t i = 0; i < blockSize; ++i)
            for (size_t ch = 0; ch < numChannels; ++ch)
                buffer[ch][i] = processSample(buffer, ch, i);
    }

private:
    
    //parameters
    SidechainInputType sidechainInputType = SidechainInputType::Feedforward;
    std::atomic<SampleType> detectorGain = 1.0;
    bool compressor = true;
    std::atomic<SampleType> thrLin = 1.0, exponent = 0.0;
    std::atomic<SampleType> exponentA = SampleType(0.0), exponentR = SampleType(0.0);
    std::atomic<SampleType> dryLin, wetLin = 1.0;

#ifdef DEBUG
    DynamicsProcessorOutputType outputType;
#endif

    //detector
    Detector<SampleType> detector;

    //filters
    NLBallisticsFilter<SampleType> nlBallisticsFilter;
    NLDET<SampleType> nlDET;

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