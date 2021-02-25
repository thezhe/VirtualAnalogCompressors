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

    //model-specific methods
    virtual void setAttack(float attackMs) noexcept = 0;
    virtual void setRelease(float releaseMs) noexcept = 0;
    virtual void prepare(const double sampleRate, const int samplesPerBlock) = 0;
    virtual void process(float* buffer) noexcept = 0;

    //common methods
    void setThreshold(float thrdB) noexcept;
    void setRatio(float ratioR) noexcept;

protected:
    
    //helper functions
    float ctf(float x) { return powf(x / thrlin, exponent); }

    //parameters
    float thrlin;
    float exponent;

    //spec
    size_t blockSize;

    //state
    float rect, bf;
};

class DECompressor : public Compressor
{
public:
    void setAttack(float attackMs) noexcept;
    void setRelease(float releaseMs) noexcept;
    void prepare(const double sampleRate, const int samplesPerBlock);

protected:

    //parameters
    float a_a, a_r;

    //spec
    float T;
};

class TPTCompressor : public Compressor
{
public:
    void setAttack(float attackMs) noexcept;
    void setRelease(float releaseMs) noexcept;
    void prepare(const double sampleRate, const int samplesPerBlock);

protected:

    //parameters
    float Ga, Gr;

    //spec
    float Tdiv2;

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
