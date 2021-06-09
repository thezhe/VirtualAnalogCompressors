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

namespace VA
{

enum class RLModelType
{
    Frohlich = 1,
    JilesAtherton
};

enum class RLTopologyType
{
    Feedforward = 1,
    Feedback
};

/** Nonlinear inductor first-order Multimode filter based on modulating cutoff model.
*
*   Note: Use for time domain effects. Implemented using Multimode1.
*/
template<typename FloatType>
class NLMM1_Time
{
public:

    /** Set the inductor nonlinearity
    *
    *   Note: A good range is [0, 500] with log tapered controls
    */
    void setNonlinearity(FloatType nonlinearityN) noexcept;

    /** Set the time in miliseconds for the step response to reach 1-1/e when nonlinearity is 0 */
    void setLinearTau(FloatType linearTauMs) noexcept;

    void setSqrtLinearOmega(FloatType sqrtLinearOmega) noexcept
    {
        omegaLinSqrt = sqrtLinearOmega;
    }

    /** Set the processing specifications */
    void prepare(FloatType sampleRate, size_t numInputChannels);

    /** Reset the internal state*/
    void reset()
    {
        mm1.reset();
        std::fill(_y.begin(), _y.end(), FloatType(0.0));
    }

    /** Process a sample given the channel */
    FloatType processSample(FloatType x, size_t channel) noexcept
    {
        FloatType& y = _y[channel];

        //Modulate Cutoff
        FloatType sqrtOmega = omegaLinSqrt + nonlinearity * std::abs((topologyType == RLTopologyType::Feedforward ? x : y));
        mm1.setOmega(sqrtOmega * sqrtOmega);

        //filter
        y = mm1.processSample(x, channel);
        return y;
    }

private:

    //parameters
    RLTopologyType topologyType = RLTopologyType::Feedback;
    RLModelType modelType = RLModelType::Frohlich;
    std::atomic<FloatType> omegaLinSqrt, nonlinearity;
    FloatType omegaLimit;

    //filter
    Multimode1<FloatType> mm1;

    //output
    std::vector<FloatType> _y{ 2 };
};

/** Nonlinear inductor first-order Multimode filter based on modulating cutoff model.
*
*   Note: Use for frequency domain effects. Implemented using Multimode1.
*/
template<typename FloatType>
class NLMM1_Freq
{
public:

    using FilterType = Multimode1FilterType;

    void setFilterType(FilterType type) noexcept
    {
        filterType = type;
    }

    void setFeedbackSaturation(bool enable) noexcept
    {
        feedbackSaturation = enable;
    }

    void setLinearCutoff(FloatType cutoffHz) noexcept;

    /// <summary>
    /// Set the inductor nonlinearity.
    /// </summary>
    /// <remarks>
    /// A good range is [0, 500] with log tapered controls.
    /// </remarks>
    void setNonlinearity(FloatType nonlinearityN) noexcept;

    /** Set the processing specifications */
    void prepare(FloatType sampleRate, size_t numInputChannels, size_t samplesPerBlock);

    /** Reset the internal state*/
    void reset();

    /// <summary>
    /// Set the number of iterations in the nonlinear zero-delay feedback solver.
    /// </summary>
    /// <remarks>
    /// Typical values are powers of 2 -- i.e., 1, 2, 4, etc.
    /// </remarks>
    void setNewtonRalphsonIterations(size_t numIterations) noexcept
    {
        nrIterations = numIterations;
    }

    /** Process a sample given the channel */
    FloatType processSample(FloatType x, size_t channel) noexcept
    {
        FloatType s = I1.getState(channel);

        //find v
        FloatType v;
        if (feedbackSaturation)
        {
            //linear y prediction
            FloatType S = s * div1plusg;
            FloatType y0 = std::fma(Glin, x, S); //G * x + S;
            FloatType y = y0;

            //nonlinear y prediction (Newton-Ralphson)
            for (size_t i = 0; i < nrIterations; ++i)
            {
                //inverse froelich kennelly
                FloatType omegaSqrt = std::fma(N, y0 > 0 ? y : -y, omegaLinSqrt);
                FloatType g = Tdiv2 * omegaSqrt * omegaSqrt;

                //f(x)
                FloatType f = y - g * (x - y) - s;

                //df(x)/dx
                FloatType fPrime = 1 + (y0 > 0 ? FloatType(-1.0) : FloatType(1.0)) * TN * omegaSqrt * (x - y) + g;

                y -= f / fPrime;
            }
            
            //calculate v
            FloatType g = Tdiv2 * MathFunctions<FloatType>::invFroelichKennelly(y, omegaLinSqrt, N);
            v = g * (x - y);
        }
        else //feedbackSaturation == false
        { 
            //calculate v
            FloatType g = Tdiv2 * MathFunctions<FloatType>::invFroelichKennelly(x, omegaLinSqrt, N);
            FloatType G = g / (FloatType(1.0) + g);
            v = (x - s) * G;
        }

        //integrate
        FloatType y = I1.processSample(v, channel);

        //output
        return filterType == Multimode1FilterType::Lowpass ? y : x - y;
    }

    /** Process a buffer */
    void process(FloatType** buffer) noexcept
    {
        for (size_t i = 0; i < blockSize; ++i)
            for (size_t ch = 0; ch < I1.getNumChannels(); ++ch)
                buffer[ch][i] = processSample(buffer[ch][i], ch);
    }

private:

    //parameters
    RLTopologyType topologyType = RLTopologyType::Feedback;
    std::atomic<FloatType> omegaLinSqrt, N, TN;
    std::atomic<size_t> nrIterations{ 4 };
    FloatType div1plusg, Glin;
    std::atomic<FilterType> filterType = FilterType::Lowpass;
    std::atomic<bool> feedbackSaturation = true;

    //filter
    Integrator<FloatType> I1;

    //spec
    size_t blockSize;
    FloatType Tdiv2{ 0.5 }, T{ 1 }, fs2{ 2 };
};

/** Ballistics filter using implemented using NLMM1_Time */
template <typename FloatType>
class NLBallisticsFilter
{
public:

    /** Set the time in miliseconds for the step response to reach 1-1/e */
    void setAttack(FloatType attackMs) noexcept;

    /** Set the inductor nonlinearity during attacks */
    void setAttackNonlinearity(FloatType nonlinearityN) noexcept;

    /** Set the time in miliseconds for inversed step response to reach 1/e */
    void setRelease(FloatType releaseMs) noexcept;

    /** Set the inductor nonlinearity during releases */
    void setReleaseNonlinearity(FloatType nonlinearityN) noexcept;

    /** Prepare the processing specifications */
    void prepare(FloatType sampleRate, size_t numInputChannels);

    /** Reset the internal state */
    void reset();

    /** Process a sample in the specified channel */
    FloatType processSample(FloatType x, size_t channel) noexcept
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
    std::atomic<FloatType> aOmegaLinSqrt = FloatType(1.0), aNonlinearity = FloatType(0.0);
    std::atomic<FloatType> rOmegaLinSqrt = FloatType(1.0), rNonlinearity = FloatType(0.0);

    //filter
    NLMM1_Time<FloatType> nlMM1;

    //state
    std::vector<FloatType> _y{ 2 };
};

/** General Envelope Filter composed of NLBallisticsFilter and DET */
template<typename FloatType>
class NLEnvelopeFilter
{
public:

    /// <summary>
    /// Set the time in milliseconds for the step response to reach 1-1/e if the nonlinearity 0
    /// </summary>
    void setAttack(FloatType attackMs) noexcept;

    /// <summary>
    /// Set the nonlinearity during attacks
    /// </summary>
    void setAttackNonlinearity(FloatType value) noexcept;

    /// <summary>
    /// Set the time in milliseconds for the inversed step response to reach 1/e if the nonlinearity is 0
    /// </summary>
    void setRelease(FloatType value) noexcept;

    /// <summary>
    /// Set the nonlinearity during releases
    /// </summary>
    void setReleaseNonlinearity(FloatType value) noexcept;

    /// <summary>
    /// Set the sensitivity in miliseconds. The filter acts as a normal ballistics filter
    /// (compressor) for large values and as DET for small values (transient designer).
    /// </summary>
    void setSensitivity(FloatType value) noexcept;

    /// <summary>
    /// Reset the internal state.
    /// </summary>
    void reset();

    /// <summary>
    /// Prepare the processing specifications
    /// </summary>
    void prepare(FloatType sampleRate, size_t numInputChannels);

    /// <summary>
    /// Process a sample given the channel
    /// </summary>
    FloatType processSample(FloatType x, size_t channel) noexcept
    {
        return nlbfFast.processSample(x, channel)-bfSlow.processSample(x, channel);
    }

private:

    //parameters
    FloatType _attackMs, _releaseMs;
    FloatType sensitivityRatio;

    //filters
    NLBallisticsFilter<FloatType> nlbfFast;
    BallisticsFilter<FloatType> bfSlow;
};

/// <summary>
/// A system which generates hysteresis loops using the Jiles-Aetherton model
/// </summary>
/// <remarks>
/// Based on: https://jatinchowdhury18.medium.com/complex-nonlinearities-episode-3-hysteresis-fdeb2cd3e3f6
/// Implementation is optimized for time domain stability and accuracy. TPT and unit delay in feedback loops
/// </remarks>
template<typename FloatType>
class Hysteresis_Time
{
public:
    
    /// <summary>
   /// Reset the internal state.
   /// </summary>
    void reset();

    /// <summary>
    /// Prepare the processing specifications
    /// </summary>
    void prepare(FloatType sampleRate, size_t numInputChannels);

    /// <summary>
    /// Set the 'drive' (a)
    /// </summary>
    /// <param name="value"></param>
    /// <returns></returns>
    void seta(FloatType value) noexcept
    {
        a = value;
        cSdiva = c * S / a;
   //     alphacSdiva = alpha * c * S / a;
    }

    /// <summary>
    /// Set the 'saturation' (S)
    /// </summary>
    /// <param name="value"></param>
    /// <returns></returns>
    void setS(FloatType value) noexcept
    {
        S = value;
        cSdiva = c * S / a;
    //    alphacSdiva = alpha * c * S / a;
    }

    /// <summary>
    /// Set the hysteresis loop width (c)
    /// </summary>
    /// <param name="value"></param>
    /// <returns></returns>
    void setc(FloatType value) noexcept
    {
        c = value;

        onemc = 1 - c;
        cSdiva = c * S / a;
     //   alphacSdiva = alpha * c * S / a;
    }

    /// <summary>
    /// Process a sample given the channel.
    /// </summary>
    FloatType processSample(FloatType x, size_t channel) noexcept
    {
        FloatType y = I1.getPrevOutput(channel); //y[n-1]
        
        //find Q
        FloatType Q = (x + y) / a;

        //find deltax
        FloatType deltax = getDeltax(x, channel);

        //find L(Q)
        FloatType L = MathFunctions<FloatType>::L(Q);
        
        //find deltay
        FloatType deltay = ~(std::signbit(deltax) ^ std::signbit(L - y));

        //find SL(Q)-y
        FloatType SLmy = S * L - y;

        //find (cS/a)L'(Q)
        FloatType dLcSdiva = MathFunctions<FloatType>::dL(Q) * cSdiva;
        
        //find dx
        FloatType dx = D1.processSample(x, channel);

        //find integrator input
        FloatType num = ((onemc * deltay * SLmy) / (onemc * deltax * k - alpha * SLmy) + dLcSdiva) * dx;
        FloatType denom = 1 - alpha * dLcSdiva;
        FloatType v = Tdiv2 * num / denom;

        //integrate
        return I1.processSample(v, channel);
    }

private:

    //helper functions
    FloatType getDeltax(FloatType x, size_t channel) noexcept
    {
        FloatType& x1 = _x1[channel];

        FloatType y = x > x1 ? FloatType(1.0) : FloatType(-1.0);
        x1 = x;
        return y;
    }

    //parameters
    std::atomic<FloatType> onemc, S, cSdiva, a, c;

    //constants
    //Source: https://github.com/jatinchowdhury18/ComplexNonlinearities/blob/master/Hysteresis/Plugin/Source/HysteresisProcessing.h
    const FloatType alpha{ 1.6e-3 }, k{ 0.47875 };

    //filters
    Differentiator<FloatType> D1;
    Integrator<FloatType> I1;

    //state
    std::vector<FloatType> _x1{ 2 }; //x[n-1]

    //spec
    FloatType Tdiv2{ 0.5 };
};




} // namespace VA


//TODO set a, c, and S need mutex?
//TODO Hysteresis 
//TODO FF NLMM1_Freq and Time
//TODO sens ratio vs sens Ms
