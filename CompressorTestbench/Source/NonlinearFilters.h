/*
  ==============================================================================
    Zhe Deng 2020
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#pragma once

#include "Filters.h"

enum class RLModelType
{
    Frohlich = 1,
    JilesAtherton
};

enum class RLTopologyType
{
    Feedforward  = 1,
    Feedback
};

/** Nonlinear inductor first-order Multimode filter based on modulating cutoff model.
*
*   Note: Use for time domain effects. Implemented using Multimode1.
*/
template<typename SampleType>
class NLMM1_Time
{
public:

    /** Set the inductor nonlinearity
    *
    *   Note: A good range is [0, 500] with log tapered controls
    */
    void setNonlinearity(SampleType nonlinearityN) noexcept;

    /** Set the time in miliseconds for the step response to reach 1-1/e when nonlinearity is 0 */
    void setLinearTau(SampleType linearTauMs) noexcept;

    void setSqrtLinearOmega(SampleType sqrtLinearOmega) noexcept
    {
        omegaLinSqrt = sqrtLinearOmega;
    }

    /** Set the processing specifications */
    void prepare(SampleType sampleRate, size_t numInputChannels);

    /** Reset the internal state*/
    void reset() 
    {
        mm1.reset();
        std::fill(_y.begin(), _y.end(), SampleType(0.0));
    }

    /** Process a sample given the channel */
    SampleType processSample(SampleType x, size_t channel) noexcept
    {
        auto& y = _y[channel];

        //Modulate Cutoff
        auto sqrtOmega = omegaLinSqrt + nonlinearity * (topologyType == RLTopologyType::Feedforward ? x : y);
        sqrtOmega = std::min(sqrtOmega, omegaLimit);
        mm1.setOmega(sqrtOmega * sqrtOmega);

        //filter
        y = mm1.processSample(x, channel);
        return y;
    }

private:

    //parameters
    RLTopologyType topologyType = RLTopologyType::Feedback;
    RLModelType modelType = RLModelType::Frohlich;
    std::atomic<SampleType> omegaLinSqrt, nonlinearity;
    SampleType omegaLimit;

    //filter
    Multimode1<SampleType> mm1;

    //output
    std::vector<SampleType> _y{ 2 };
};

/** Nonlinear inductor first-order Multimode filter based on modulating cutoff model.
*
*   Note: Use for frequency domain effects. Implemented using Multimode1.
*/
template<typename SampleType>
class NLMM1_Freq
{
public:

    using FilterType = Multimode1FilterType;

    void setFilterType(FilterType type) noexcept
    {
        filterType = type;
    }

    void setLinearCutoff(SampleType cutoffHz) noexcept;
    
    /// <summary>
    /// Set the inductor nonlinearity.
    /// </summary>
    /// <remarks>
    /// A good range is [0, 500] with log tapered controls.
    /// </remarks>
    void setNonlinearity(SampleType nonlinearityN) noexcept;

    /** Set the processing specifications */
    void prepare(SampleType sampleRate, size_t numInputChannels, size_t samplesPerBlock);

    /** Reset the internal state*/
    void reset();

    /// <summary>
    /// Set the number of iterations to run on the internal Newton-Ralphson routine.
    /// </summary>
    /// <remarks>
    /// Typical values are powers of 2 -- i.e., 1, 2, 4, etc.
    /// 
    /// </remarks>
    void setNewtonRalphsonIterations(size_t numIterations) noexcept
    {
        nrIterations = numIterations;
    }

    /** Process a sample given the channel */
    SampleType processSample(SampleType x, size_t channel) noexcept
    {
        SampleType& s = _s1[channel];

        //linear y prediction
        SampleType S = s * div1plusg;
        SampleType y0 = std::fma(G, x, S); //G * x + S;
        SampleType y = y0;


        //nonlinear y prediction (Newton-Ralphson)
        if (y0 > 0)
        {
            for (size_t i = 0; i < nrIterations; ++i)
            {
                SampleType omegaSqrt = std::fma(N, y, omegaLinSqrt);
                SampleType g = Tdiv2 * omegaSqrt * omegaSqrt;
                SampleType f = y - g * (x - y) - s;
                SampleType fPrime = 1 - TN * omegaSqrt * (x - y) + g;

                y -= f / fPrime;
            }
        }
        else
        {
            for (size_t i = 0; i < nrIterations; ++i)
            {
                SampleType omegaSqrt = std::fma(N, -y, omegaLinSqrt);
                SampleType g = Tdiv2 * omegaSqrt * omegaSqrt;
                SampleType f = y - g * (x - y) - s;
                SampleType fPrime = 1 + TN * omegaSqrt * (x - y) + g;

                y -= f / fPrime;
            }
        }

        //calculate v
        SampleType omegaSqrt = std::fma(N, y, omegaLinSqrt); //N * y + omegaLinSqrt;
        SampleType g = Tdiv2 * omegaSqrt * omegaSqrt;

        SampleType v = g * (x - y);
        
        //integrate
        y = v + s;
        s = y + v;
        return filterType == Multimode1FilterType::Lowpass ? y : x - y;
    }

    /** Process a buffer */
    void process(SampleType** buffer) noexcept
    {
        for (size_t i = 0; i < blockSize; ++i)
            for (size_t ch = 0; ch < _s1.size(); ++ch)
                buffer[ch][i] = processSample(buffer[ch][i], ch);
    }

private:

    //parameters
    RLTopologyType topologyType = RLTopologyType::Feedback;
    std::atomic<SampleType> omegaLinSqrt, N, TN;
    std::atomic<size_t> nrIterations{ 4 };
    SampleType div1plusg, G;
    FilterType filterType = FilterType::Lowpass;

    //state
    std::vector<SampleType> _s1{ 2 };

    //spec
    size_t blockSize;
    SampleType Tdiv2{ 0.5 }, T{ 1 }, fs2{ 2 };
};

/** Ballistics filter using implemented using NLMM1_Time */
template <typename SampleType>
class NLBallisticsFilter
{
public:

    /** Set the time in miliseconds for the step response to reach 1-1/e */
    void setAttack(SampleType attackMs) noexcept;

    /** Set the inductor nonlinearity during attacks */
    void setAttackNonlinearity(SampleType nonlinearityN) noexcept;

    /** Set the time in miliseconds for inversed step response to reach 1/e */
    void setRelease(SampleType releaseMs) noexcept;

    /** Set the inductor nonlinearity during releases */
    void setReleaseNonlinearity(SampleType nonlinearityN) noexcept;

    /** Prepare the processing specifications */
    void prepare(SampleType sampleRate, size_t numInputChannels);

    /** Reset the internal state */
    void reset();

    /** Process a sample in the specified channel */
    SampleType processSample(SampleType x, size_t channel) noexcept
    {
        auto& y = _y[channel];

        //branching
        nlMM1.setSqrtLinearOmega(x < y ? rOmegaLinSqrt : aOmegaLinSqrt);
        nlMM1.setNonlinearity(x < y ? rNonlinearity : aNonlinearity);

        //filter
        y = nlMM1.processSample(x, channel);
        return y;
    }

private:

    //parameters
    std::atomic<SampleType> aOmegaLinSqrt = SampleType(1.0), aNonlinearity = SampleType(0.0);
    std::atomic<SampleType> rOmegaLinSqrt = SampleType(1.0), rNonlinearity = SampleType(0.0);

    //filter
    NLMM1_Time<SampleType> nlMM1;

    //state
    std::vector<SampleType> _y{ 2 };
};

/** General Envelope Filter composed of NLBallisticsFilter and DET */
template<typename SampleType>
class NLEnvelopeFilter
{
public:

    /** Set the time in miliseconds for the step response to reach 1-1/e */
    void setAttack(SampleType attackMs) noexcept;

    /** Set the inductor nonlinearity during attacks */
    void setAttackNonlinearity(SampleType nonlinearityN) noexcept;

    /** Set the time in miliseconds for inversed step response to reach 1/e */
    void setRelease(SampleType releaseMs) noexcept;

    /** Set the inductor nonlinearity during releases */
    void setReleaseNonlinearity(SampleType nonlinearityN) noexcept;

    /// <summary>
    /// Set the sensitivity. The filter acts as a normal Ballistics Filter
    /// (compressor) for large values and as DET for small values (transient designer).
    /// </summary>
    void setSensitivity(SampleType sensitivity) noexcept;

    /// <summary>
    /// Reset the internal state.
    /// </summary>
    void reset();

    /// <summary>
    /// Prepare the processing specifications
    /// </summary>
    void prepare(SampleType sampleRate, size_t numInputChannels);

    SampleType processSample(SampleType x, size_t channel) noexcept
    {
        return nlbfFast.processSample(x, channel) - bfSlow.processSample(x, channel);
    }

private:

    //parameters
    SampleType _attackMs, _releaseMs;
    SampleType sensitivityRatio;

    //filters
    NLBallisticsFilter<SampleType> nlbfFast;
    BallisticsFilter<SampleType> bfSlow;
};

//TODO Hysteresis 
//TODO FF NLMM1_Freq and Time

