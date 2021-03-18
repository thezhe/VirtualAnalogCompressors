/*
  ==============================================================================
    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "NonlinearFilters.h"


#pragma region Abstract Classes

template <typename SampleType>
class Compressor
{
public:

    //platform dependent SIMD
    using SIMD = xsimd::simd_type<SampleType>;
    
    //common methods
    void setThreshold(SampleType thrdB) noexcept;

    void setRatio(SampleType ratioR) noexcept;

    void setWet(SampleType wetdB) noexcept;

    void setDry(SampleType drydB) noexcept;
 
    //model-specific methods
    virtual void prepare(const double sampleRate, const int samplesPerBlock) = 0;
    virtual void setAttack(SampleType attackMs) noexcept = 0;
    virtual void setRelease(SampleType releaseMs) noexcept = 0;
    virtual void process(SampleType* interleaved) noexcept = 0;

protected:
    
    //helper functions
    SIMD ctf(SIMD x) noexcept { return xsimd::select(x > thrlin, xsimd::pow(x / thrlin, exponent), SIMD(1.0)); }

    //parameters
    SIMD thrlin = SIMD(1.0), exponent = SIMD(0.0);
    SIMD dryLin = SIMD(0.0), wetLin = SIMD(1.0);

    //spec
    size_t blockSize = 2;

    //state
    SIMD bf;
};

template<typename SampleType>
class Compressor_IIR : public Compressor<SampleType>
{
public:

    void prepare(const double sampleRate, const int samplesPerBlock);

    void setAttack(SampleType attackMs) noexcept;

    void setRelease(SampleType releaseMs) noexcept;

protected:

    BallisticsFilter_IIR<SampleType> ballisticsFilter_IIR;
};

template<typename SampleType>
class Compressor_TPTz : public Compressor<SampleType>
{
public:

    void prepare(const double sampleRate, const int samplesPerBlock);
    
    void setAttack(SampleType attackMs) noexcept;
    
    void setRelease(SampleType releaseMs) noexcept;

protected:

    BallisticsFilter_TPTz<SampleType> ballisticsFilter_TPTz;
};

template<typename SampleType>
class Compressor_TPT : public Compressor<SampleType>
{
public:

    void prepare(const double sampleRate, const int samplesPerBlock);

    void setAttack(SampleType attackMs) noexcept;

    void setRelease(SampleType releaseMs) noexcept;

protected:

    BallisticsFilter_TPT<SampleType> ballisticsFilter_TPT;
};

#pragma endregion

#pragma region Linear 

template<typename SampleType>
class FFVCA_IIR final : public Compressor_IIR<SampleType>
{
public:
    void process(SampleType* interleaved) noexcept
    {
        for (size_t i = 0; i < blockSize; i+=SIMD::size)
        {
            SIMD x = xsimd::load_aligned(&interleaved[i]);
            //ballistics filter
            bf = ballisticsFilter_IIR.processSample(x);
            //compressor transfer function and mixing
            xsimd::store_aligned(&interleaved[i], (dryLin * x) + (wetLin * x * ctf(bf)));
        }
    }
};

template<typename SampleType>
class FFVCA_TPTz final : public Compressor_TPTz<SampleType>
{
public:
    void process(SampleType* interleaved) noexcept
    {
        for (size_t i = 0; i < blockSize; i += SIMD::size)
        {
            SIMD x = xsimd::load_aligned(&interleaved[i]);
            //ballistics filter
            bf = ballisticsFilter_TPTz.processSample(x);
            //compressor transfer function and mixing
            xsimd::store_aligned(&interleaved[i], (dryLin * x) + (wetLin * x * ctf(bf)));
        }
    }
};

template< typename SampleType >
class FFVCA_TPT final : public Compressor_TPT<SampleType>
{
public:
    void process(SampleType* interleaved) noexcept
    {
        for (size_t i = 0; i < blockSize; i += SIMD::size)
        {
            SIMD x = xsimd::load_aligned(&interleaved[i]);
            //ballistics filter
            bf = ballisticsFilter_TPT.processSample(x);
            //compressor transfer function and mixing
            xsimd::store_aligned(&interleaved[i], (dryLin * x) + (wetLin * x * ctf(bf)));
        }
    }
};

template<typename SampleType>
class FBVCA_IIR final : public Compressor_IIR<SampleType>
{
public:

    void process(SampleType* interleaved) noexcept
    {
        for (size_t i = 0; i < blockSize; i += SIMD::size)
        {
            SIMD x = xsimd::load_aligned(&interleaved[i]);
            //ballistics filter
            bf = ballisticsFilter_IIR.processSample(y);
            //store output
            y = x * ctf(bf);
            //compressor transfer function and mixing
            xsimd::store_aligned(&interleaved[i], (dryLin * x) + (wetLin * y));
        }
    }

private:

    //output
    SIMD y;
};

template<typename SampleType>
class FBVCA_TPTz final : public Compressor_TPTz<SampleType>
{
public:

    void process(SampleType* interleaved) noexcept
    {
        for (size_t i = 0; i < blockSize; i += SIMD::size)
        {
            SIMD x = xsimd::load_aligned(&interleaved[i]);
            //ballistics filter
            bf = ballisticsFilter_TPTz.processSample(y);
            //store output
            y = x * ctf(bf);
            //compressor transfer function and mixing
            xsimd::store_aligned(&interleaved[i], (dryLin * x) + (wetLin * y));
        }
    }

private:

    //output
    SIMD y;
};

/*template<typename SampleType>
class FBVCA_TPT final : public TPTCompressor<SampleType>
{

};*/

#pragma endregion

#pragma region Nonlinear

template<typename SampleType>
class FFVCA_RL_Modulating_TPTz final : public Compressor<SampleType>
{
public:

    void prepare(const double sampleRate, const int samplesPerBlock);
    
    void setAttack(SampleType attackMs) noexcept;
    
    void setRelease(SampleType releaseMs) noexcept;

    void setLinearCutoffRL(SampleType cutoffHz) noexcept;

    void setSaturationRL(SampleType saturationConstant) noexcept;

    void setIntensityRL(SampleType intensitydB) noexcept;

    void process(SampleType* interleaved) noexcept
    {
        for (size_t i = 0; i < blockSize; i += SIMD::size)
        {
            SIMD x = xsimd::load_aligned(&interleaved[i]);
            //ballistics filter
            bf = ballisticsFilter.processSample(x);
            //nonlinear low pass
            bf = rl.processSample(bf);
            //compressor transfer function and mixing
            xsimd::store_aligned(&interleaved[i], (dryLin * x) + (wetLin * x * ctf(bf)));
        }
    }

private:
    
    //sidechain filters
    BallisticsFilter_IIR<SampleType> ballisticsFilter;
    RL_Modulating_TPTz<SampleType> rl;

};

#pragma endregion


//TODO stereo link
//TODO optimize set[Parameter] Methods

