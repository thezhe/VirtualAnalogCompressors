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

/** Nonlinear RL low-pass filter based on modulating cutoff model
*
*   Note: Use for time domain effects. Implemented using MM1.
*/
template<typename SampleType>
class NLMM1_Time
{
public:
    
    NLMM1_Time() {}

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

    //filter
    Multimode1<SampleType> mm1;

    //output
    std::vector<SampleType> _y{ 2 };
};

/** Ballistics filter using implemented using RL_Time */
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

/** Differential Envelope Technology using NLMM1_Time */
template <typename SampleType>
class NLDET
{
public:
    
    NLDET() {}

    void setTau(SampleType tauMs) noexcept
    {
        tau = tauMs;
        envFast.setLinearTau(tauMs);
    }

    void setSensitivity(SampleType sensS) noexcept
    {
        envSlow.setLinearTau((SampleType(1.0) + sensS) * tau);
    }

    void setNonlinearity(SampleType nonlinearityN) noexcept
    {
        envFast.setNonlinearity(nonlinearityN);
    }

    /** Reset the internal state */
    void reset();

    /** Set the processing specifications */
    void prepare(SampleType sampleRate, size_t numInputChannels);

    /** Process a sample */
    SampleType processSample(SampleType x, size_t channel) noexcept
    {
        return envFast.processSample(x, channel) - envSlow.processSample(x, channel);
    }

private:

    //parameters
    SampleType tau;

    //filters
    NLMM1_Time<SampleType> envFast, envSlow;
};

//TODO RL_Frequency, Hysteresis 

