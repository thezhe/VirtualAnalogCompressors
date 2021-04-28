/*
  ==============================================================================
    Functions, constants, and classes for utility -- inspired by JUCE.
    
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

/** Lookup table function approximation */
template<typename FloatType>
class LookupTable
{
public:

    LookupTable() {}

    /** Create a LoopupTable that approximates function f in the range [a, b] using n samples */
    LookupTable(const std::function<FloatType(FloatType)> f, FloatType a, FloatType b, size_t n)
    {
        prepare(f, a, b, n);
    }

    /** Create a LoopupTable that approximates function f in the range [a, b] using n samples */
    void prepare(const std::function<FloatType(FloatType)> f, FloatType a, FloatType b, size_t n);

    /** Process a sample after values less than a are mapped to a */
    FloatType processSampleMinChecked(FloatType x)
    {
        return processSampleUnchecked(std::max(_a, x));
    }

    /** Process a sample after values greater than b are mapped to b */
    FloatType processSampleMaxChecked(FloatType x)
    {
        return processSampleUnchecked(std::min(x, _b));
    }

    /** Process a sample after it is clamped in range [a,b]*/
    FloatType processSampleChecked(FloatType x)
    {
        return processSampleUnchecked(MathFunctions<FloatType>::clamp(x, _a, _b));   
    }

    /** Process a sample without checking bounds */
    FloatType processSampleUnchecked(FloatType x)
    {
        FloatType fracIdx = MathFunctions<FloatType>::invLerp(_a, divbma , x) * maxIdx;
        FloatType a, t;
        t = modf(fracIdx, &a);
        return MathFunctions<FloatType>::lerp(fLUT[a], fLUT[a + 1], t);
    } 

private:

    //spec
    FloatType _a, _b, maxIdx, divbma;

    //internal lookup table
    std::vector<FloatType> fLUT;

};

namespace Decibels
{
    /** Convert decibels to linear gain */
    template <typename Type>
    static Type decibelsToGain(Type decibels, Type minusInfinitydB = -100.0)
    {
        return decibels > minusInfinitydB ? 
            pow(Type(10.0), decibels * Type(0.05)): //gain = 10^(dB/20)
            Type(0.0);
    }

    /** Convert linear gain to decibels */
    template <typename Type>
    static Type gainToDecibels(Type gain)
    {
        return gain > Type(0.0) ?
            max(minusInfinitydB, static_cast<Type> (log10(gain)) * Type(20.0)) : //dB = 20*log10(gain)
            minusInfinityDb;
    }
}

template<typename FloatType>
struct MathFunctions
{

    /** Clamp x to range [a,b] */
    static constexpr FloatType clamp(FloatType x, FloatType a, FloatType b)
    {
        return std::min(std::max(x, a), b);
    }

    /** Linearly interpolate from a to b given parameter t */
    static constexpr FloatType lerp(FloatType a, FloatType b, FloatType t)
    {
        return fma(b - a, t, a);
    }

    /** Calculate t given an linearly interpolated value y in the range [a,b] and the values of a and 1/(b-a) */
    static constexpr FloatType invLerp(FloatType a, FloatType divbma, FloatType y)
    {
        return (y - a) * divbma;
    }
    
    /** Calculate root of function f using Newton Ralphson with initial guess x0 and n number of iterations */
    static constexpr FloatType netwonRalphson
    (
        std::function<FloatType(FloatType)> f, 
        std::function <FloatType(FloatType)> fPrime,
        FloatType x0, 
        size_t n
    )
    {
        FloatType x = x0;
        
        for (size_t i = 0; i < n; ++i)
            x = x - f(x) / fPrime(x);
        return x;
    }

    /** Convert a tau time in miliseconds to angular frequency */
    static constexpr FloatType tauToOmega(FloatType tauMs)
    {
        return FloatType(1000.0) / tauMs;
    }

    /** Convert a dynamics processor transfer function ratio to exponent */
    static constexpr FloatType ratioToExponent(FloatType ratioR)
    {
        return FloatType(1.0) / ratioR - FloatType(1.0);
    }

    /** Compressor Transfer Function */
    static constexpr FloatType ctf(FloatType x, FloatType thrLin, FloatType exponent)
    {
        return x > thrLin ? pow(x / thrLin, exponent) : FloatType(1.0);
    }

    /** Transient Transfer Function */
    static constexpr FloatType ttf(FloatType x, FloatType thrLin, FloatType exponentA, FloatType exponentR)
    {
        return x > 0 ? ctf(x, thrLin, exponentA) : ctf(-x, thrLin, exponentR);
    }

};

template <typename FloatType>
struct MathConstants
{
    /** A predefined value for Pi */
    static constexpr FloatType pi = static_cast<FloatType> (3.141592653589793238L);

    /** A predefined value for Pi */
    static constexpr FloatType pi2 = static_cast<FloatType> (2 * 3.141592653589793238L);
};

//TODO static functions (ctfs, ttfs, detector, etc)