/*
  ==============================================================================

    VCA Compressor Classes
    Zhe Deng 2021

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#pragma region Abstract Classes

//template <typename SampleType>
class Compressor
{
public:

    //common methods
    void setThreshold(float thrdB) noexcept;
    void setRatio(float ratioR) noexcept;
    void setWet(float wetdB) noexcept;
    void setDry(float drydB) noexcept;
 

    //model-specific methods
    virtual void prepare(const double sampleRate, const int samplesPerBlock) = 0;
    virtual void process(float* buffer) noexcept = 0;
    virtual void setAttack(float attackMs) noexcept = 0;
    virtual void setRelease(float releaseMs) noexcept = 0;

protected:
    
    //helper functions
    float ctf(float x) noexcept { return x < thrlin ? 1 : pow(x/thrlin, exponent); }

    //parameters
    float thrlin = 1;
    float exponent;
    float dryLin = 0, wetLin = 1;

    //spec
    size_t blockSize = 2;

    //state
    float rect, bf;
};

//template<typename SampleType>
class DECompressor : public Compressor//<SampleType>
{
public:

    void prepare(const double sampleRate, const int samplesPerBlock);
    void setAttack(float attackMs) noexcept;
    void setRelease(float releaseMs) noexcept;

protected:

    //parameters
    float a_a = 0.5f, a_r = 0.5f;

    //spec
    float T = 1.f;
};

//template<typename SampleType>
class TPTCompressor : public Compressor//<SampleType>
{
public:
    void setAttack(float attackMs) noexcept;
    void setRelease(float releaseMs) noexcept;
    void prepare(const double sampleRate, const int samplesPerBlock);

protected:

    //parameters
    float Ga = 0.5f, Gr = 0.5f;

    //spec
    float Tdiv2 = 0.5f;

    //state
    float s;
};

#pragma endregion

#pragma region Final Classes

//template<typename SampleType>
class FFVCA_Trad final : public DECompressor//<SampleType>
{
public:
    void process(float* buffer) noexcept
    {
        for (size_t i = 0; i < blockSize; ++i)
        {
            //rectifier
            rect = fabsf(buffer[i]);
            //ballistics filter
            float a = (rect < bf) ? a_r : a_a;
            bf = a * bf + (1.f - a) * rect;
            //compressor transfer function and mixing
            buffer[i] = dryLin * buffer[i] + wetLin * buffer[i] * ctf(bf);
        }
    }
};

//template<typename SampleType>
class FFVCA_TPTz final : public TPTCompressor//<SampleType>
{
public:
    void process(float* buffer) noexcept
    {
        for (size_t i = 0; i < blockSize; ++i)
        {
            //rectifier
            rect = fabsf(buffer[i]);
            //ballistics filter
            float G = (rect < bf) ? Gr : Ga;
            float v = (rect - s) * G;
            bf = v + s;
            s = bf + v;
            //compressor transfer function and mixing
            buffer[i] = dryLin * buffer[i] + wetLin * buffer[i] * ctf(bf);
        }
    }
};

//template< typename SampleType >
class FFVCA_TPT final : public TPTCompressor//<SampleType>
{
public:
    void process(float* buffer) noexcept
    {
        for (size_t i = 0; i < blockSize; ++i)
        {
            //rectifier
            rect = fabsf(buffer[i]);
            //ballistics filter
            float G = (s > rect) ? Gr : Ga;
            float v = (rect - s) * G;
            bf = v + s;
            s = bf + v;
            //compressor transfer function
            buffer[i] = dryLin * buffer[i] + wetLin * buffer[i] * ctf(bf);
        }
    }
};

//template<typename SampleType>
class FBVCA_Trad final : public DECompressor//<SampleType>
{
public:
    void process(float* buffer) noexcept
    {
        for (size_t i = 0; i < blockSize; ++i)
        {
            //rectifier
            rect = fabsf(y);
            //ballistics filter
            float a = (rect < bf) ? a_r : a_a;
            bf = a * bf + (1.f - a) * rect;
            //compressor transfer function
            buffer[i] = dryLin * buffer[i] + wetLin * buffer[i] * ctf(bf);
            //store last output
            y = buffer[i];
        }
    }
private:
    float y;
};

//template<typename SampleType>
class FBVCA_TPTz final : public TPTCompressor//<SampleType>
{
public:
    void process(float* buffer) noexcept
    {
        for (size_t i = 0; i < blockSize; ++i)
        {
            //rectifier
            rect = fabsf(y);
            //ballistics filter
            float G = (s > rect) ? Gr : Ga;
            float v = (rect - s) * G;
            bf = v + s;
            s = bf + v;
            //compressor transfer function
            buffer[i] = dryLin * buffer[i] + wetLin * buffer[i] * ctf(bf);
            //store last output
            y = buffer[i];
        }
    }
private:
    float y;
};

/*template<typename SampleType>
class FBVCA_TPT final : public TPTCompressor<SampleType>
{

};*/

#pragma endregion

//TODO SIMD optimize branches, conditionals
//TODO template classes