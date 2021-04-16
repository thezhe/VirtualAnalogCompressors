/*
  ==============================================================================
    Dynamic processors.
    
    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#pragma once

#include "NonlinearFilters.h"

enum class CompressorSidechainType
{
    Feedforward = 1,
    Feedback,
    External
};

/** Dynamic range compressor */
template <typename SampleType>
class Compressor: public Processor<SampleType>
{
public:

    using SidechainType = CompressorSidechainType;

    void setSidechainType(SidechainType sidechainType) noexcept
    {
        sidechain = sidechainType;
    }

    void setThreshold(SampleType thrdB) noexcept;

    void setRatio(SampleType ratioR) noexcept;

    /** Set the input gain in dB */
    void setInputGain(SampleType inputdB) noexcept;

    /** Set the gain of the processed signal in dB*/
    void setWetGain(SampleType wetdB) noexcept;

    /** Set the gain of the unprocessed signal in dB */
    void setDryGain(SampleType drydB) noexcept;

    /** Enable or disable stereo linking
    *
    *   Note: Stereo linking applies the same processing to all channels
    */
    void setStereoLink(bool enableStereoLink) noexcept
    {
        detector.setStereoLink(enableStereoLink);
    }

    void setDetectorMode(DetectorType mode) noexcept
    {
        detector.setMode(mode);
    }

    void setAttack(SampleType attackMs) noexcept;
    
    void setRelease(SampleType releaseMs) noexcept;

    void setRL(bool enableRL) noexcept 
    { 
        RL = enableRL; 
    }

    void setRMS(bool RMSenable)
    {
        ballisticsFilter.setRMS(RMSenable);
    }

    void setLinearTauRL(SampleType linearTauMs) noexcept;

    void setSaturationRL(SampleType saturationConstant) noexcept;

    void prepare(double sampleRate, int samplesPerBlock, int numInputChannels);
    
    void reset()
    {
        detector.reset();
        ballisticsFilter.reset();
        frohlichRL.reset();
    }

    SIMD processSample(SIMD x) noexcept
    {
        //detector and ballistics filter
        b = ballisticsFilter.processSample(
            inLin*detector.processSample(
            sidechain==SidechainType::Feedforward ?
            x :
            y
            )
        );
        //RL
        if (RL) b = frohlichRL.processSample(b);
        //ctf        
        y = x * ctf(b);
        //mix
        return (dryLin * x) + (wetLin * y);
    }

    void process(SampleType* interleaved) noexcept
    {
        for (size_t i = 0; i < interleavedSize; i += SIMD::size)
        {
            SIMD x = xsimd::load_aligned(&interleaved[i]);
            xsimd::store_aligned(&interleaved[i], processSample(x));
        }
    }

private:
    
    //helper functions
    SIMD ctf(SIMD x) noexcept 
    { 
        return xsimd::select(x > thrlin, xsimd::pow(x / thrlin, exponent), SIMD(1.0)); 
    }
    
    //parameters
    SIMD thrlin = SIMD(1.0), exponent = SIMD(0.0);
    SIMD inLin = SIMD(1.0), dryLin, wetLin = SIMD(1.0);
    SidechainType sidechain = SidechainType::Feedforward;
    bool RL = false;
    
    //filters
    Detector<SampleType> detector;
    BallisticsFilter<SampleType> ballisticsFilter;
    RL_Modulating_Riemann<SampleType> frohlichRL;

    //output
    SIMD b, y;   

    //spec
    size_t interleavedSize = 2;
};

enum class TransientDesignerSidechainType
{
    Feedforward = 1,
    Feedback
};

///** Transient Designer */
//template <typename SampleType>
//class TransientDesigner : public Processor<SampleType>
//{
//public:
//
//    using SidechainType = CompressorSidechainType;
//    
//    /** Set the input gain in dB */
//    void setInputGain(SampleType inputdB) noexcept;
//
//    /** Set the gain of the processed signal in dB*/
//    void setWetGain(SampleType wetdB) noexcept;
//
//    /** Set the gain of the unprocessed signal in dB */
//    void setDryGain(SampleType drydB) noexcept;
//
//    /** Enable or disable stereo linking
//    *
//    *   Note: Stereo linking applies the same processing to all channels
//    */
//    void setStereoLink(bool enableStereoLink) noexcept
//    {
//        det.setStereoLink(enableSteroLink);
//    }
//
//    void setAttack(SampleType attackMs) noexcept;
//
//    void setRelease(SampleType releaseMs) noexcept;
//
//
//    void setRL(bool enableRL) noexcept
//    {
//        RL = enableRL;
//    }
//
//    void setLinearTauRL(SampleType linearTauMs) noexcept;
//
//    void setSaturationRL(SampleType saturationConstant) noexcept;
//
//    void prepare(double sampleRate, int samplesPerBlock, int numInputChannels);
//
//    void reset()
//    {
//        det.reset();
//    }
//
//    SIMD processSample(SIMD x) noexcept
//    {
//        //ballistics filter
//        b = det.processSample(x);
//        //ctf        
//        y = x * ttf(b);
//        //mix
//        return (dryLin * x) + (wetLin * y);
//    }
//
//private:
//
//    //helper functions
//    SIMD ttf(SIMD x) noexcept 
//    { 
//        return xsimd::select(
//            x > SIMD(0.0), 
//            SIMD(1.0) + ctf(x * attackGain), 
//            SIMD(1.0) + releaseGain
//        ); 
//    }
//
//    //parameters
//    SIMD inLin = SIMD(1.0), dryLin, wetLin = SIMD(1.0);
//    SIMD attackGain, releaseGain;
//
//    SidechainType sidechain = SidechainType::Feedforward;
//    bool RL = false;
//
//    //filters
//    DET<SampleType> det;
//
//    //output
//    SIMD b, y;
//};

//TODO sidechain support
//TODO expansion (upwards and downwards) and compression (upwards and downwards)
//TODO soft knee
//TODO second order DET for better noise floor?
//TODO input gain
//TODO ttf define or use ctf with thresh at 0
//TODO waveshaping transfer functions
//TODO nonlinearityRL normalize to [0,1]