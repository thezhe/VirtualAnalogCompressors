/*
  ==============================================================================

    VCA Compressor Classes
    Zhe Deng 2021

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#pragma region Abstract Classes

class Compressor
{
public:

    //common methods
    void setThreshold(float thrdB) noexcept;
    void setRatio(float ratioR) noexcept;

    //model-specific methods
    virtual void prepare(const double sampleRate, const int samplesPerBlock) = 0;
    virtual void process(float* buffer) noexcept = 0;
    virtual void setAttack(float attackMs) noexcept = 0;
    virtual void setRelease(float releaseMs) noexcept = 0;

protected:
    
    //helper functions
    float ctf(float x) noexcept { return powf(x/thrlin, exponent); }

    //parameters
    float thrlin = 1.f;
    float exponent;

    //spec
    size_t blockSize = 2;

    //state
    float rect, bf;
};

class DECompressor : public Compressor
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

class TPTCompressor : public Compressor
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

class FFVCA_Trad : public DECompressor
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
            //compressor transfer function
            if (bf > thrlin)
                buffer[i] *= ctf(bf);
        }
    }
};

class FFVCA_TPTz : public TPTCompressor
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
            //compressor transfer function
            if (bf > thrlin)
                buffer[i] *= ctf(bf);
        }
    }
};

class FFVCA_TPT : public TPTCompressor
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
            if (bf > thrlin)
                buffer[i] *= ctf(bf);
        }
    }
};
