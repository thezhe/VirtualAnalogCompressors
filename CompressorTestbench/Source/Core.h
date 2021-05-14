/*
  ==============================================================================
    Core tools for column-major SISD audio filter design -- written in C++17.

    Defines:
    _ITERATOR_DEBUG_LEVEL=0  <=>  Avoid checking index bounds during debug builds (msvc)

    Naming convention:
    m <=> -
    div <=> /
    d2 <=> second derivative
    private variables may have underscore prefix for clarity 
    'x' is reserved for function inputs
    'y' is reserved for function outputs

    Examples:
    1.) adivamb <=> a / (a - b)
    2.) void MyClass::setA(int A)
        {
            _A = A;
        }

    Zhe Deng 2021
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#pragma once

#include <algorithm>
#include <atomic>
#include <cmath>
#include <functional>
#include <vector>

namespace VA
{

/// <summary>
/// Function approximation via lookup table.
/// </summary>
/// <typeparam name="FloatType">Must be float or double.</typeparam>
template<typename FloatType>
class LookupTable
{
public:

    /** Create a LookupTable to approximate function f in the range [a, b] using n samples */
    constexpr LookupTable(const std::function<FloatType(FloatType)> f, FloatType a, FloatType b, size_t n)
    {
        prepare(f, a, b, n);
    }

    /// <summary>
    /// Prepare a LookupTable to approximate function f in the range [a,b] using n samples.
    /// </summary>
    /// <remarks>
    /// Must be solveable during compile time.
    /// </remarks>
    /// <typeparam name="FloatType">Must be float or double.</typeparam>
    constexpr void prepare(const std::function<FloatType(FloatType)> f, FloatType a, FloatType b, size_t n);

    /** Process a sample after values less than a are mapped to a */
    constexpr FloatType processSampleMinChecked(FloatType x) const noexcept
    {
        return processSampleUnchecked(std::max(_a, x));
    }

    /** Process a sample after values greater than b are mapped to b */
    constexpr FloatType processSampleMaxChecked(FloatType x) const noexcept
    {
        return processSampleUnchecked(std::min(x, _b));
    }

    /** Process a sample after it is clamped in range [a,b]*/
    constexpr FloatType processSampleChecked(FloatType x) const noexcept
    {
        return processSampleUnchecked(std::clamp(x, _a, _b));
    }

    /** Process a sample without checking bounds */
    constexpr FloatType processSampleUnchecked(FloatType x) const noexcept
    {
        //get fractional index
        FloatType fracIdx = MathFunctions<FloatType>::invLerp(divbma, adivamb, x) * maxIdx;

        //get lerp arguments
        FloatType a;
        FloatType t = std::modf(fracIdx, &a);

        //lerp
        return MathFunctions<FloatType>::lerp(fLUT[size_t(a)], fLUT[size_t(a) + size_t(1)], t);
    }

private:

    //internal lookup table
    std::vector<FloatType> fLUT;

    //spec
    FloatType _a, _b, divbma, adivamb;
    FloatType maxIdx;
};

/// <summary>
/// Basic delay line optimized for column-major processing
/// </summary>
/// <remarks>
/// Circular addressing based on: https://github.com/hamiltonkibbe/FxDSP/blob/master/FxDSP/src/CircularBuffer.c
/// </remarks>
/// <typeparam name="FloatType">Must be float or double</typeparam>
template<typename FloatType>
class DelayLine
{
public:

    constexpr void prepare(FloatType sampleRate, size_t nChannels, FloatType maxDelayMs)
    {

    }

    FloatType read() noexcept
    {

    }

    void write(FloatType x) noexcept
    {

    }

    void reset()
    {

    }

private:
    std::vector<std::vector<FloatType>> buffer;
};

/// <summary>
/// Trapezoidal Integrator (Transposed Direct Form II)
/// </summary>
/// <remarks>
/// The integrator input, v, must be scaled by T/2 and 
/// any (possibly pre-warped) gain factors before processing.
/// </remarks>
/// <typeparam name="FloatType"></typeparam>
template<typename FloatType>
class Integrator
{
public:
    /// <summary>
    /// Prepare the processing specifications
    /// </summary>
    void prepare(size_t numInputChannels)
    {
        _s.resize(numInputChannels);
        _y.resize(numInputChannels);
        reset();
    }

    /// <summary>
    /// Reset the internal state
    /// </summary>
    void reset()
    {
        std::fill(_s.begin(), _s.end(), FloatType(0.0));
        std::fill(_y.begin(), _y.end(), FloatType(0.0));
    }

    /// <summary>
    /// Get the number of channels
    /// </summary>
    size_t getNumChannels() const noexcept
    {
        return _s.size();
    }

    /// <summary>
    /// Get the state given the channel
    /// </summary>
    FloatType getState(size_t channel)
    {
        return _s[channel];
    }

    FloatType getPrevOutput(size_t channel)
    {
        return _y[channel];
    }
    /// <summary>
    /// Process the integrator input given the channel.
    /// </summary>
    /// <param name="v">Integrator input</param>
    FloatType processSample(FloatType v, size_t channel) noexcept
    {
        FloatType& s = _s[channel];
        FloatType& y = _y[channel];

        //integrator
        y = v + s;
        s = y + v;

        return y;
    }

private:

    //state
    std::vector<FloatType> _s{ 2 };

    //output
    std::vector<FloatType> _y{ 2 };
};

/// <summary>
/// Trapezoidal Differentiator (Direct Form I)
/// </summary>
/// <typeparam name="FloatType"></typeparam>
template<typename FloatType>
class Differentiator
{
public:

    void prepare(FloatType sampleRate, size_t numInputChannels)
    {
        fs2 = FloatType(2.0) * sampleRate;

        _x1.resize(numInputChannels);
        _y1.resize(numInputChannels);
        
        reset();
    }

    void reset()
    {
        std::fill(_x1.begin(), _x1.end(), FloatType(0.0));
        std::fill(_y1.begin(), _y1.end(), FloatType(0.0));
    }

    /// <summary>
    /// Process a sample given the channel
    /// </summary>
    /// <remarks>
    /// Unlike <c>Integrator</c> the input is internally scaled by twice the sampling rate (fs2) 
    /// </remarks>
    FloatType processSample(FloatType x, size_t channel) noexcept
    {
        FloatType& x1 = _x1[channel];
        FloatType& y1 = _y1[channel];

        //differentiator
        y1 = fs2 * (x - x1) - y1;
        x1 = x;

        return y1;
    }

private:
    FloatType fs2{ 2 };

    std::vector<FloatType> _y1{ 2 };
    std::vector<FloatType> _x1{ 2 };
};

/// <summary>
/// Common static functions and tools for analyzing them
/// </summary>
/// <remarks>
/// Most functions are not optimized. Consider using <c>LookupTable</c> to approximate functions 
/// with transcendentals or other expensive operations.
/// </remarks>
/// <typeparam name="FloatType">Must be float or double</typeparam>
template<typename FloatType>
struct MathFunctions
{
    /// <summary>
    /// The value of 1/L using the Froelich Kennelly nonlinear inductor model.
    /// Use when the angular cutoff frequency is equal to R/L. 
    /// </summary>
    /// <param name="V">Voltage across the inductor</param>
    /// <param name="OmegaLinSqrt">The angular cutoff frequency if N=0 or V=0</param>
    /// <param name="N">Nonlinearity factor</param>
    /// <returns>Angular cutoff frequency</returns>
    static constexpr FloatType invFroelichKennelly(FloatType V, FloatType OmegaLinSqrt, FloatType N) noexcept
    {
        FloatType OmegaSqrt = std::fma(N, std::abs(V), OmegaLinSqrt); //N * abs(V) + OmegaLinSqrt;
        FloatType Omega = OmegaSqrt * OmegaSqrt;
        return Omega;
    }

    static constexpr FloatType L(FloatType x) noexcept 
    {
        if (std::abs(x) < 0.0001)
            return x / 3;
        return FloatType(1.0) / std::tanh(x) - FloatType(1.0) / x;
    }

    static constexpr FloatType dL(FloatType x) noexcept
    {
        if (std::abs(x) < 0.0001)
            return 1 / 3;
        return 1 - FloatType(1.0) / std::pow(std::tanh(x), FloatType(2.0)) + FloatType(1.0) / (x * x);
    }

    static constexpr FloatType d2L(FloatType Q) noexcept
    {
        return FloatType(0.0);
    }

    /** Hardware and framework independent function to disable denormals
    *
    *   Note: Idea based on
    *   https://www.earlevel.com/main/2019/04/19/floating-point-denormals/
    */
    static constexpr void noDenormals(FloatType** buffer, size_t numChannels, size_t blockSize)
    {
        for (size_t ch = 0; ch < numChannels; ++ch)
            for (size_t i = 0; i < blockSize; ++i)
                buffer[ch][i] = buffer[ch][i];
    }

    /** Pre-warp an angular cutoff frequency (omega) given sampleRate*2 and samplingPeriod/2 */
    static constexpr FloatType preWarp(FloatType omega, FloatType fs2, FloatType Tdiv2) noexcept
    {
        return fs2 * std::tan(omega * Tdiv2);
    }

    /** Convert decibels to linear gain */
    static constexpr FloatType decibelsToGain(FloatType decibels, FloatType minusInfinitydB = -100.0) noexcept
    {
        return decibels > minusInfinitydB ?
            std::pow(FloatType(10.0), decibels * FloatType(0.05)) : //gain = 10^(dB/20)
            FloatType(0.0);
    }

    /** Convert linear gain to decibels */
    static constexpr FloatType gainToDecibels(FloatType gain, FloatType minusInfinitydB = FloatType(-100.0)) noexcept
    {
        return gain > FloatType(0.0) ?
            std::max(minusInfinitydB, static_cast<FloatType> (std::log10(gain)) * FloatType(20.0)) : //dB = 20*log10(gain)
            minusInfinitydB;
    }

    /** Linearly interpolate from a to b given parameter t */
    static constexpr FloatType lerp(FloatType a, FloatType b, FloatType t) noexcept
    {
        return std::fma(b - a, t, a);
    }

    /** Calculate t given an linearly interpolated value y in the range [a,b] and the values of 1/(b-a) and a/(a-b) */
    static constexpr FloatType invLerp(FloatType divbma, FloatType adivamb, FloatType y) noexcept
    {
        return std::fma(y, divbma, adivamb);
    }

    /** Convert a tau time in miliseconds to angular frequency */
    static constexpr FloatType tauToOmega(FloatType tauMs) noexcept
    {
        return FloatType(1000.0) / tauMs;
    }

    /** Convert a dynamics processor transfer function ratio to exponent */
    static constexpr FloatType ratioToExponent(FloatType ratioR) noexcept
    {
        return FloatType(1.0) / ratioR - FloatType(1.0);
    }

    /** Compressor Transfer Function with adjustable knee in log domain*/
    
    /// <summary>
    /// Compressor transfer function with adjustable knee
    /// </summary>
    /// <remarks>
    /// Function is in log domain -- requires conversion to a linear gain
    /// </remarks>
    /// <param name="thrdB">Threshold</param>
    /// <param name="W">Knee width</param>
    /// <param name="R">Ratio</param>
    static constexpr FloatType ctfKnee_dB(FloatType x, FloatType thrdB, FloatType W, FloatType R)
    {
        FloatType xmthrdB2 = (x - thrdB) * 2;
        
        //Source: https://www.eecs.qmul.ac.uk/~josh/documents/2012/GiannoulisMassbergReiss-dynamicrangecompression-JAES2012.pdf
        if (xmthrdB2 < -W)
            return x;
        else if (xmthrdB2 > W)
            return thrdB + (x - thrdB) / R;
        else //2*abs(x-thrdB) <= W
            return x + (1 / R - 1) * pow(x - thrdB + W / 2, 2) / (2 * W);
    }

    /** Compressor Transfer Function */
    static constexpr FloatType ctf(FloatType x, FloatType thrLin, FloatType exponent) noexcept
    {
        return x > thrLin ? std::pow(x / thrLin, exponent) : FloatType(1.0);
    }

    /** Transient Transfer Function */
    static constexpr FloatType ttf(FloatType x, FloatType thrLin, FloatType exponentA, FloatType exponentR) noexcept
    {
        return x > 0 ? ctf(x, thrLin, exponentA) : ctf(-x, thrLin, exponentR);
    }
};

/// <summary>
/// Common constants
/// </summary>
/// <typeparam name="FloatType">Must be float or double</typeparam>
template <typename FloatType>
struct MathConstants
{
    /** A predefined value for pi */
    static constexpr FloatType pi = static_cast<FloatType> (3.141592653589793238L);

    /** A predefined value for 2*pi */
    static constexpr FloatType pi2 = static_cast<FloatType> (2 * 3.141592653589793238L);
};

} // namespace VA


//TODO consider globals (T, Tdiv2, etc)
//TODO circular buffer using bit masking addressing
//TODO finish noDenormals and ctf with knee parameter
//TODO finish differentiator
//TODO finish d2L
//TODO branch free else ifs