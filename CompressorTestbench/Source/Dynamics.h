/*
  ==============================================================================
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

template <typename SampleType>
class Compressor
{
public:

    using SIMD = xsimd::simd_type<SampleType>;
    
    using SidechainType = CompressorSidechainType;

    void setSidechainType(SidechainType sidechainType) noexcept
    {
        sidechain = sidechainType;
    }

    void setThreshold(SampleType thrdB) noexcept;

    void setRatio(SampleType ratioR) noexcept;

    void setWet(SampleType wetdB) noexcept;

    void setDry(SampleType drydB) noexcept;
    
    void setAttack(SampleType attackMs) noexcept;
    
    void setRelease(SampleType releaseMs) noexcept;

    void enableRL(bool enable) noexcept { RLenabled = enable; }

    void setLinearTauRL(SampleType linearTauMs) noexcept;

    void setSaturationRL(SampleType saturationConstant) noexcept;

    void prepare(const double sampleRate, const int samplesPerBlock);

    void reset()
    {
        ballisticsFilter.reset();
        frohlichRL.reset();
    }

    SIMD processSample(SIMD x) noexcept
    {
        //ballistics filter
        b = ballisticsFilter.processSample(
            sidechain==SidechainType::Feedforward ?
            x :
            y
        );
        //RL
        if (RLenabled) b = frohlichRL.processSample(b);
        //ctf        
        y = x * ctf(b);
        //mix
        return (dryLin * x) + (wetLin * y);
    }

    /** Process an interleaved, SIMD-aligned buffer */
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
    SIMD ctf(SIMD x) noexcept { return xsimd::select(x > thrlin, xsimd::pow(x / thrlin, exponent), SIMD(1.0)); }

    //spec
    size_t interleavedSize = 2;
    
    //parameters
    SIMD thrlin = SIMD(1.0), exponent = SIMD(0.0);
    SIMD dryLin = SIMD(0.0), wetLin = SIMD(1.0);
    SidechainType sidechain = SidechainType::Feedforward;
    bool RLenabled = false;
    
    //filters
    BallisticsFilter<SampleType> ballisticsFilter;
    RL_Modulating_Riemann<SampleType> frohlichRL;

    //output
    SIMD b, y;   
};


//TODO stereo link
//TODO soft knee
//TODO optimize set[Parameter] Methods