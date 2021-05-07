/*
  ==============================================================================
    Core tools for column-major SISD audio filter design -- written in C++20.

    Defines:
    _ITERATOR_DEBUG_LEVEL=0  <=>  Avoid checking index bounds during debug builds (msvc)

    Naming convention:
    m <=> -
    div <=> /
    private variables may have underscore prefix for clarity 

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
#include <numbers>
#include <vector>

/// <summary>
/// Function approximation via lookup table.
/// </summary>
/// <typeparam name="FloatType">Must be float or double.</typeparam>
template<typename FloatType>
class LookupTable
{
public:

    /** Create a LookupTable to approximate function f in the range [a, b] using n samples */
    consteval LookupTable(const std::function<FloatType(FloatType)> f, FloatType a, FloatType b, size_t n)
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
    consteval void prepare(const std::function<FloatType(FloatType)> f, FloatType a, FloatType b, size_t n);

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
        FloatType fracIdx = MathFunctions<FloatType>::invLerp(divbma, adivamb , x) * maxIdx;
        
        //get lerp arguments
        FloatType a;
        FloatType t = std::modf(fracIdx, &a);

        //lerp
        return MathFunctions<FloatType>::lerp(fLUT[a], fLUT[a + 1], t);
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
/// Common static functions
/// </summary>
/// <typeparam name="FloatType">Must be float or double</typeparam>
template<typename FloatType>
struct MathFunctions
{
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

    /** Compressor Transfer Function with adjustable knee in log domain
    *
    *   Note: Branch-free
    */
    static constexpr FloatType ctfKnee_dB(FloatType x, FloatType thrLin, FloatType ratioR, FloatType kneeWidth)
    {
        return FloatType(0.0);
    }

    /** Compressor Transfer Function */
    static constexpr FloatType ctf(FloatType x, FloatType thrLin, FloatType exponent) noexcept
    {
        return x > thrLin ? pow(x / thrLin, exponent) : FloatType(1.0);
    }

    /** Transient Transfer Function */
    static constexpr FloatType ttf(FloatType x, FloatType thrLin, FloatType exponentA, FloatType exponentR) noexcept
    {
        return x > 0 ? ctf(x, thrLin, exponentA) : ctf(-x, thrLin, exponentR);
    }
};

/** Extra constants not included in std::numbers */
template <typename FloatType>
struct MathConstants
{
    /** A predefined value for 2*pi */
    static constexpr FloatType pi2 = static_cast<FloatType> (2 * 3.141592653589793238L);
};

//TODO consider globals (T, T_2, etc)
//TODO circular buffer using bit masking addressing
//TODO Using a lambda or functor to inline custom functions: blog.demofox.org/2015/02/25/avoiding-the-performance-hazzards-of-stdfunction/
//TODO finish noDenormals and ctf with knee parameter