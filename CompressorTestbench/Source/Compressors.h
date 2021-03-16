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
#include "Filters.h"



#pragma region Abstract Classes

template <typename SampleType>
class Compressor
{
public:
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
    SIMD thrlin = 1;
    SIMD exponent;

    SIMD dryLin = 0, wetLin = 1;

    //spec
    size_t blockSize = 2;

    //state
    SIMD rect, bf;
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


    //ballistics filter
    BallisticsFilter_TPT<SampleType> ballisticsFilter_TPTz;
};

template<typename SampleType>
class Compressor_TPT : public Compressor<SampleType>
{
public:

    void prepare(const double sampleRate, const int samplesPerBlock);

    void setAttack(SampleType attackMs) noexcept;

    void setRelease(SampleType releaseMs) noexcept;

protected:

    //ballistics filter
    BallisticsFilter_TPT<SampleType> ballisticsFilter_TPT;
};

#pragma endregion

#pragma region Final Classes

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
            xsimd::store_aligned(&interleaved[i], (dryLin * x) + (wetLin * y);
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
            xsimd::store_aligned(&interleaved[i], (dryLin * x) + (wetLin * y);
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

//TODO SIMD optimize branches, conditionals
//TODO template classes
//TODO stereo link
//TODO optimize set[Parameter] Methods
//TODO SIMD exponentials, transcendentals, non JUCE SIMD