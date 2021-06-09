/*
  ==============================================================================
    Core tools for column-major SISD audio filter design -- written in C++17.

    Designed for cross-platform, cross-framework, and hardware compatibility.

    DEFINES:
    _ITERATOR_DEBUG_LEVEL=0  <=>  Avoid checking index bounds during debug builds (msvc)

    Naming convention:
    m <=> -
    div <=> /
    d2 <=> second derivative
    private variables may have underscore prefix for clarity 
    'x' is reserved for function inputs
    'y' is reserved for function outputs
    'FloatType' is reserved for template identifiers that can be either float or double

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
/// Function approximation via lookup table
/// </summary>
/// <remarks>
/// For best performance create and initialize static objects
/// </remarks>
template<typename FloatType>
class LookupTable
{
public:

    /// <summary>
    /// Default Constructor. Use with <c>prepare</c>.
    /// </summary>
    LookupTable() {};

    /// <summary>
    /// Construct a <c>LookupTable</c> to approximate function <c>f</c> in the range [<c>a</c>, <c>b</c>] using <c>n</c> samples
    /// </summary>
    constexpr LookupTable(const std::function<FloatType(FloatType)> f, FloatType a, FloatType b, size_t n)
    {
        prepare(f, a, b, n);
    }

    /// <summary>
    /// Prepare the <c>LookupTable</c> to approximate function <c>f</c> in the range [<c>a</c>, <c>b</c>] using <c>n</c> samples
    /// </summary>
    /// <remarks>
    /// Must be solveable during compile time
    /// </remarks>
    constexpr void prepare(const std::function<FloatType(FloatType)> f, FloatType a, FloatType b, size_t n);

    /// <summary>
    /// Process a sample after <c>x</c> values less than <с>a</с> are mapped to <c>a</c>
    /// </summary>
    constexpr FloatType processSampleMinChecked(FloatType x) const noexcept
    {
        return processSampleUnchecked(std::max(_a, x));
    }

    /// <summary>
    /// Process a sample after <c>x</c> values more than <с>b</с> are mapped to <c>b</c>
    /// </summary>
    constexpr FloatType processSampleMaxChecked(FloatType x) const noexcept
    {
        return processSampleUnchecked(std::min(x, _b));
    }

    /// <summary>
    /// Process a sample after clamping <c>x</c> to the interval [<c>a</c>, <c>b</c>]
    /// </summary>
    constexpr FloatType processSampleChecked(FloatType x) const noexcept
    {
        return processSampleUnchecked(std::clamp(x, _a, _b));
    }

    /// <summary>
    /// Process a sample without checking bounds.
    /// Use this if you know that x will be in the interval [<c>a</c>, <c>b</c>].
    /// </summary>
    constexpr FloatType processSampleUnchecked(FloatType x) const noexcept
    {
        //get fractional index
        FloatType fracIdx = MathFunctions<FloatType>::invLerp(divbma, adivamb, x) * maxIdx;

        //get lerp arguments
        FloatType a;
        FloatType t = std::modf(fracIdx, &a);

        //LUT lerp
        return MathFunctions<FloatType>::lerp(fLUT[size_t(a)], fLUT[size_t(a) + size_t(1)], t);
    }

private:
    
    //spec
    FloatType _a{ -1 }, _b{ 1 }, divbma{ 1 / (_b - _a) }, adivamb{ _a / (_a - _b) };
    FloatType maxIdx{ fLUT.size() - FloatType(1.0) };

    //function lookup table
    std::vector<FloatType> fLUT;

};

/// <summary>
/// Basic delay line optimized for column-major processing
/// </summary>
/// <remarks>
/// Circular addressing based on: https://github.com/hamiltonkibbe/FxDSP/blob/master/FxDSP/src/CircularBuffer.c
/// </remarks>
template<typename FloatType>
class DelayLine
{
public:

    constexpr void prepare(FloatType sampleRate, size_t nChannels, FloatType maxDelayMs)
    {

    }

    FloatType read() const noexcept
    {

    }

    void write(FloatType x) noexcept
    {

    }

    void reset()
    {

    }

private:

    //circular buffer
    std::vector<std::vector<FloatType>> cBuf;
};

/// <summary>
/// Trapezoidal Integrator (Transposed Direct Form II)
/// </summary>
/// <remarks>
/// The integrator input, v, must be scaled by T/2 and 
/// any (possibly pre-warped) gain factors before processing.
/// </remarks>
template<typename FloatType>
class Integrator
{
public:

    /// <summary>
    /// Prepare the processing specifications
    /// </summary>
    void prepare(size_t numInputChannels);

    /// <summary>
    /// Reset the internal state
    /// </summary>
    void reset();

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
    FloatType getState(size_t channel) const noexcept
    {
        return _s[channel];
    }

    /// <summary>
    /// Get the output at sample n-1 given the channel
    /// </summary>
    FloatType getPrevOutput(size_t channel) const noexcept
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
template<typename FloatType>
class Differentiator
{
public:

    /// <summary>
    /// Prepare the processing specifications
    /// </summary>
    void prepare(FloatType sampleRate, size_t numInputChannels);

    /// <summary>
    /// Reset the internal state
    /// </summary>
    void reset();

    /// <summary>
    /// Process a sample given the channel
    /// </summary>
    /// <remarks>
    /// Unlike <c>Integrator</c>, the input is internally scaled by twice the sampling rate (<c>fs2</c>) 
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

    //state
    std::vector<FloatType> _y1{ 2 };
    std::vector<FloatType> _x1{ 2 };

    //spec
    FloatType fs2{ 2 };
};

/// <summary>
/// Common static functions and tools for analyzing them
/// </summary>
/// <remarks>
/// Most functions are not optimized. Consider using <c>LookupTable</c> to approximate functions 
/// with transcendentals or other expensive operations.
/// </remarks>
template<typename FloatType>
struct MathFunctions
{
    /// <summary>
    /// The value of 1/L using the Froelich-Kennelley anhysteretic B-H relation.
    /// Use when the angular cutoff frequency is R/L (e.g. First-order multimode filter). 
    /// </summary>
    /// <remarks>
    /// Based on: https://www.dafx.de/paper-archive/2015/DAFx-15_submission_68.pdf
    /// </remarks>
    /// <param name="V">Voltage across the inductor</param>
    /// <param name="OmegaLinSqrt">The angular cutoff frequency when N=0 or V=0. No checks are done to make sure this parameter is valid.</param>
    /// <param name="N">Nonlinearity. A good interval is [0, 5000].</param>
    /// <returns>Angular cutoff frequency</returns>
    static constexpr FloatType invFroelichKennelly(FloatType V, FloatType OmegaLinSqrt, FloatType N) noexcept
    {
        FloatType OmegaSqrt = std::fma(N, std::abs(V), OmegaLinSqrt); //N * abs(V) + OmegaLinSqrt;
        return OmegaSqrt * OmegaSqrt;
    }

    /// <summary>
    /// Jiles-Atherton Langevin Function
    /// </summary>
    /// <remarks>
    /// Source: https://dafx2019.bcu.ac.uk/papers/DAFx2019_paper_3.pdf
    /// </remarks>
    static constexpr FloatType L(FloatType x) noexcept 
    {
        if (std::abs(x) < 0.0001)
            return x / 3;
        return FloatType(1.0) / std::tanh(x) - FloatType(1.0) / x;
    }

    /// <summary>
    /// First derivative of the Jiles-Atherton Langevin Function 
    /// </summary>
    static constexpr FloatType dL(FloatType x) noexcept
    {
        if (std::abs(x) < 0.0001)
            return 1 / 3;
        return 1 - FloatType(1.0) / std::pow(std::tanh(x), FloatType(2.0)) + FloatType(1.0) / (x * x);
    }

    /// <summary>
    /// Second derivative of the Jiles-Atherton Langevin Function
    /// </summary>
    static constexpr FloatType d2L(FloatType x) noexcept
    {
        return FloatType(0.0);
    }

    /// <summary>
    /// Modify a buffer to prevent denormal processing
    /// </summary>
    /// <remarks>
    /// Source: https://www.earlevel.com/main/2019/04/19/floating-point-denormals/
    /// </remarks>
    static constexpr void noDenormals(FloatType** buffer, size_t numChannels, size_t blockSize)
    {
        for (size_t ch = 0; ch < numChannels; ++ch)
            for (size_t i = 0; i < blockSize; ++i)
                buffer[ch][i] = buffer[ch][i];
    }

    /// <summary>
    /// Pre-warp an angular cutoff frequency given twice the sampling rate and and half the sampling period
    /// </summary>
    /// <param name="Omega">Angular cutoff frequency</param>
    /// <param name="fs2">Twice the sampling rate</param>
    /// <param name="Tdiv2">Half the sampling period</param>
    static constexpr FloatType preWarp(FloatType Omega, FloatType fs2, FloatType Tdiv2) noexcept
    {
        return fs2 * std::tan(Omega * Tdiv2);
    }

    /// <summary>
    /// Convert decibels to linear gain
    /// </summary>
    /// <param name="minusInfinitydB">Values below this parameter will return 0</param>
    static constexpr FloatType decibelsToGain(FloatType x, FloatType minusInfinitydB = FloatType(-100.0)) noexcept
    {
        return x > minusInfinitydB ?
            std::pow(FloatType(10.0), x * FloatType(0.05)) : //10^(x/20)
            FloatType(0.0);
    }

    /// <summary>
    /// Convert linear gain to decibels
    /// </summary>
    /// <param name="minusInfinitydB">The lowest possible return value</param>
    static constexpr FloatType gainToDecibels(FloatType x, FloatType minusInfinitydB = FloatType(-100.0)) noexcept
    {
        return x > FloatType(0.0) ?
            std::max(minusInfinitydB, std::log10(x) * FloatType(20.0)) : //20*log10(x)
            minusInfinitydB;
    }

    /// <summary>
    /// Linearly interpolate in the range [<c>a</c>, <c>b</c>] given the interpolation parameter <c>t</c>
    /// </summary>
    static constexpr FloatType lerp(FloatType a, FloatType b, FloatType t) noexcept
    {
        return std::fma(b - a, t, a);
    }

    /// <summary>
    /// Calculate t given the linearly interpolated value <c>y</c> in the range [<c>a</c>, <c>b</c>] and the values of 1/(<c>b</c>-<c>a</c>) and <c>a</c>/(<c>a</c>-<c>b</c>)
    /// </summary>
    static constexpr FloatType invLerp(FloatType divbma, FloatType adivamb, FloatType y) noexcept
    {
        return std::fma(y, divbma, adivamb);
    }

    /// <summary>
    /// Compressor transfer function with adjustable knee
    /// </summary>
    /// <remarks>
    /// Input (<c>x</c>), return (<c>y</c>), and threshold (<c>thr</c>) are in decibels.
    /// Source: https://www.eecs.qmul.ac.uk/~josh/documents/2012/GiannoulisMassbergReiss-dynamicrangecompression-JAES2012.pdf
    /// </remarks>
    /// <param name="thr">Threshold</param>
    /// <param name="W">Knee width</param>
    /// <param name="R">Multiplicative inverse of the ratio</param>
    static constexpr FloatType ctf(FloatType x, FloatType thr, FloatType W, FloatType divR)
    {
        FloatType xmthr2 = (x - thr) * 2;
        
        //Three-way branch
        if (xmthr2 < -W)
            return x;
        else if (xmthr2 > W)
            return std::fma(x - thr, divR, thr); //(x-thr)/R + thr
        else //2*abs(x-thr) <= W
        {
            FloatType temp = std::fma(W, FloatType(0.5), x - thr); //x - thr + W/2
            return std::fma(temp * temp / (2 * W), divR - 1, x); //x + (divR - 1)*temp*temp/(2*W)
        }
    }
};

/// <summary>
/// Common constants
/// </summary>
template <typename FloatType>
struct MathConstants
{
    //A predefined value for pi
    static constexpr FloatType pi = FloatType(3.141592653589793238L);

    //A predefined value for 2*pi
    static constexpr FloatType pi2 = FloatType(2 * 3.141592653589793238L);
};

} // namespace VA

//TODO consider globals (T, Tdiv2, etc)
//TODO circular buffer using bit masking addressing
//TODO finish noDenormals
//TODO differentiator transposed DF2
//TODO finish d2L
//TODO branch free else ifs
//TODO include sources and based ons
//TODO fast modf and increasing speed on std functions
//TODO std::fma