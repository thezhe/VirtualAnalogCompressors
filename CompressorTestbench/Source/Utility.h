/*
  ==============================================================================
    Static functions and constants for utility. Inspired by sections from JUCE.
    
    Zhe Deng 2021
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#pragma once

namespace Decibels
{
    //lowest dB value
    const double minusInfinitydB = -100.0;

    /** Convert decibels to linear gain */
    template <typename Type>
    static Type decibelsToGain(Type decibels)
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

template <typename FloatType>
struct MathConstants
{
    /** A predefined value for Pi */
    static constexpr FloatType pi = static_cast<FloatType> (3.141592653589793238L);

    /** A predefined value for 2 * Pi */
    static constexpr FloatType twoPi = static_cast<FloatType> (2 * 3.141592653589793238L);
};