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

namespace VA
{

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

enum class DynamicsProcessorCtfType
{
    Traditional = 1,
    InductorBH
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
template <typename FloatType>
class DynamicsProcessor
{
public:

    using FilterType = Multimode1FilterType;
    using SidechainInputType = DynamicsProcessorSidechainInputType;
    using CtfType = DynamicsProcessorCtfType;

    /// <summary>
    /// Set the sidechain input 
    /// </summary>
    void setSidechainInputType(SidechainInputType type) noexcept
    {
        sidechainInputType = type;
    }

    /// <summary>
    /// Set the compressor transfer function
    /// </summary>
    void setCtfType(CtfType type) noexcept
    {
        ctfType = type;
    }

#ifdef DEBUG

    /// <summary>
    /// Output different nodes of the processing chain for analysis
    /// </summary>
    void setOutputType(DynamicsProcessorOutputType type) noexcept
    {
        outputType = type;
    }

    /// <summary>
    /// Move the ctf before the envelope filter
    /// </summary>
    void setCtfBeforeEnvelope(bool enable) noexcept
    {
        ctfBeforeEnvelope = enable;
    }

#endif

    /// <summary>
    /// Use the average of the sidechain input's channels for the sidechain
    /// </summary>
    void setStereoLink(bool enable) noexcept;

    /// <summary>
    /// Set the frequency-weighting filter 
    /// </summary>
    void setPreFilterType(DetectorPreFilterType type) noexcept;

    /// <summary>
    /// Set the rectifier function
    /// </summary>
    void setRectifierType(DetectorRectifierType type) noexcept;

    /// <summary>
    /// Set the ctf threshold in decibels
    /// </summary>
    void setThreshold(FloatType value) noexcept
    {
        thr = value;
    }

    /// <summary>
    /// Set the ctf knee width. A value of 0 is a hard knee.
    /// </summary>
    void setKnee(FloatType value) noexcept 
    {
        knee = value;
    }

    /// <summary>
    /// Set the ctf ratio.
    /// </summary>
    void setRatio(FloatType value) noexcept
    {
        divRatio = FloatType(1.0) / value;;
    }

    /// <summary>
    /// Set the attack time of the envelope filter in miliseconds
    /// </summary>
    void setAttack(FloatType value) noexcept;
    
    /// <summary>
    /// Set the nonlinearity of the envelope filter during attacks
    /// </summary>
    void setAttackNonlinearity(FloatType value) noexcept;

    /// <summary>
    /// Set the release of the envelope filter in miliseconds
    /// </summary>
    void setRelease(FloatType releaseMs) noexcept;

    /// <summary>
    /// Set the nonlinearity of the envelope filter during releases
    /// </summary>
    void setReleaseNonlinearity(FloatType value) noexcept;

    /// <summary>
    /// Set the sensitivity of the envelope filter. A value of 0 is a ballistics filter.
    /// </summary>
    void setSensitivity(FloatType sensitivity) noexcept;

    /// <summary>
    /// Set the gain of the processed signal in decibels
    /// </summary>
    void setWetGain(FloatType value) noexcept;

    /// <summary>
    /// Set the gain of the unprocessed signal in decibels
    /// </summary>
    void setDryGain(FloatType drydB) noexcept;

    /// <summary>
    /// Reset the internal state
    /// </summary>
    void reset();

    /// <summary>
    /// Prepare the processing specifications
    /// </summary>
    void prepare(FloatType sampleRate, size_t samplesPerBlock, size_t numInputChannels);

    /// <summary>
    /// Process a sample and a sidechain sample given the channel
    /// </summary>
    /// <param name="sc">Sidechain sample</param>
    FloatType processSample(FloatType x, FloatType sc, size_t channel) noexcept
    {
        //Detector
        FloatType d = detector.processSample(sc, channel);

        //Smoothing
        FloatType env = nlEF.processSample(d, channel);
        //FloatType env = bf.processSample(d, channel);

        //Transfer Function (output gain will be between -100dB and 0dB)
        FloatType envdB = gainToDecibelsLUT.processSampleUnchecked(env);
        FloatType tf = 0;
        switch (ctfType)
        {
        case CtfType::Traditional:
            tf = decibelsToGainLUT.processSampleChecked(MathFunctions<FloatType>::ctf(envdB, thr, knee, divRatio)) / env;
            break;
        default: //CtfType::InductorBH
            //tf = decibelsToGainLUT.processSampleChecked(hysteresis.processSample(envdB, channel)) / env;
            break;
        }

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
        default: //DynamicsProcessorOutputType::Normal:
            return x * tf;
            break;
        }

#endif
        
        //Output for release builds
        return x * tf;
    }

    /// <summary>
    /// Process a buffer
    /// </summary>
    void process(FloatType** buffer) noexcept
    {
        //seperate cases for stereoLink, stereoLink feedback, no SL, noSL feedback
        if (stereoLink)
        {
            switch (sidechainInputType)
            {
            case SidechainInputType::Feedforward:
                for (size_t i = 0; i < blockSize; ++i)
                {
                    FloatType sc = monoConverter.processFrame(buffer, i);
                    for (size_t ch = 0; ch < numChannels; ++ch)
                        buffer[ch][i] = std::fma(dryLin, buffer[ch][i], wetLin * processSample(buffer[ch][i], sc, ch));
                }
                break;
            case SidechainInputType::Feedback:
                for (size_t i = 0; i < blockSize; ++i)
                {
                    FloatType sc = monoConverter.processFrame(_y);
                    for (size_t ch = 0; ch < numChannels; ++ch)
                    {
                        _y[ch] = processSample(buffer[ch][i], sc, ch);
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
                        buffer[ch][i] = std::fma(dryLin, buffer[ch][i], wetLin * processSample(buffer[ch][i], buffer[ch][i], ch));
                }
                break;
            case SidechainInputType::Feedback:
                for (size_t i = 0; i < blockSize; ++i)
                {
                    for (size_t ch = 0; ch < numChannels; ++ch)
                    {
                        _y[ch] = processSample(buffer[ch][i], _y[ch], ch);
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
    std::atomic<SidechainInputType> sidechainInputType{ SidechainInputType::Feedforward };
    std::atomic<CtfType> ctfType{ CtfType::Traditional };
    std::atomic<bool> stereoLink = true;
    std::atomic<FloatType> thr{ 0 }, divRatio{ 1 }, knee{ 0 }, dryLin{ 0 }, wetLin{ 1 };

    //filters
    MonoConverter<FloatType> monoConverter;
    Detector<FloatType> detector;
    NLEnvelopeFilter<FloatType> nlEF;
    BallisticsFilter<FloatType> bf;
    Hysteresis_Time<FloatType> hysteresis;

    //state
    std::vector<FloatType> _y{ 2 };

    //spec
    size_t blockSize{ 512 }, numChannels{ 2 };

    //LUTs
    static LookupTable<FloatType> decibelsToGainLUT;
    static LookupTable<FloatType> gainToDecibelsLUT;

#ifdef DEBUG

    //Output a specific node
    std::atomic<DynamicsProcessorOutputType> outputType = DynamicsProcessorOutputType::Normal;
    
    //Ctf location
    bool ctfBeforeEnvelope = false;

#endif
};

template<typename FloatType>
LookupTable<FloatType> DynamicsProcessor<FloatType>::decibelsToGainLUT
(
    [](FloatType x) { return MathFunctions<FloatType>::decibelsToGain(x); },
    -100.0,
    0.0,
    128
);

template<typename FloatType>
LookupTable<FloatType> DynamicsProcessor<FloatType>::gainToDecibelsLUT
(
    [](FloatType x) { return MathFunctions<FloatType>::gainToDecibels(x); },
    0,
    1,
    128
);

} // namespace VA

//TODO sidechain support
//TODO expansion (upwards and downwards) and compression (upwards and downwards)
//TODO waveshaping transfer functions
//TODO nonlinearity normalize to [0,1]
//TODO optimize stereo link to one channel internally
//TODO subtract env in db domain