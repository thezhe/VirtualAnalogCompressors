/*
  ==============================================================================

    Utility.cpp
    Created: 21 Apr 2021 2:34:07pm
    Author:  tigr2

  ==============================================================================
*/

#include "Core.h"

template<typename FloatType>
void LookupTable<FloatType>::prepare(const std::function<FloatType(FloatType)> f, FloatType a, FloatType b, size_t n)
{
    fLUT.resize(n);
    maxIdx = n - 1;

    for (size_t i = 0; i < n; ++i)
        fLUT[i] = f(MathFunctions<FloatType>::lerp(a, b, FloatType(i) / maxIdx));
}

template class LookupTable<float>;
template class LookupTable<double>;