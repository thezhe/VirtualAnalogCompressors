/*
  ==============================================================================
    Zhe Deng 2021
    thezhefromcenterville@gmail.com

    This file is part of CompressorTestBench which is released under the MIT license.
    See file LICENSE or go to https://github.com/thezhe/VirtualAnalogCompressors for full license details.
  ==============================================================================
*/

#include "Core.h"

namespace VA
{

//==============================================================================

template<typename FloatType>
constexpr void LookupTable<FloatType>::prepare(const std::function<FloatType(FloatType)> f, FloatType a, FloatType b, size_t n)
{
    //spec
    _a = a;
    _b = b;
    divbma = FloatType(1.0) / (_b - _a);
    adivamb = _a / (_a - _b);
    maxIdx = n - 1;

    //function lookup table
    fLUT.resize(n);
    for (size_t i = 0; i < n; ++i)
        fLUT[i] = f(MathFunctions<FloatType>::lerp(_a, _b, i / maxIdx));
}

// "How can I avoid linker errors with my template classes?" 
//https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
template class LookupTable<float>;
template class LookupTable<double>;

//==============================================================================

template<typename FloatType>
void VA::Integrator<FloatType>::prepare(size_t numInputChannels)
{
    //state
    _s.resize(numInputChannels);
    _y.resize(numInputChannels);

    reset();
}

template<typename FloatType>
void Integrator<FloatType>::reset()
{
    //state
    std::fill(_s.begin(), _s.end(), FloatType(0.0));
    std::fill(_y.begin(), _y.end(), FloatType(0.0));
}

template class Integrator<float>;
template class Integrator<double>;

//==============================================================================

template<typename FloatType>
void Differentiator<FloatType>::prepare(FloatType sampleRate, size_t numInputChannels)
{
    //spec
    fs2 = FloatType(2.0) * sampleRate;

    //state
    _x1.resize(numInputChannels);
    _y1.resize(numInputChannels);

    reset();
}

template<typename FloatType>
void Differentiator<FloatType>::reset()
{
    //state
    std::fill(_x1.begin(), _x1.end(), FloatType(0.0));
    std::fill(_y1.begin(), _y1.end(), FloatType(0.0));
}

template class Differentiator<float>;
template class Differentiator<double>;

//==============================================================================

} // namespace VA
