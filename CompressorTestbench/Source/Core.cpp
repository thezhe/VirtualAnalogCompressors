/*
  ==============================================================================

    Utility.cpp
    Created: 21 Apr 2021 2:34:07pm
    Author:  tigr2

  ==============================================================================
*/

#include "Core.h"

namespace VA
{

template<typename FloatType>
constexpr void LookupTable<FloatType>::prepare(const std::function<FloatType(FloatType)> f, FloatType a, FloatType b, size_t n)
{
    //spec
    _a = a;
    _b = b;
    divbma = FloatType(1.0) / (_b - _a);
    adivamb = _a / (_a - _b);
    maxIdx = n - size_t(1);

    //internal lookup table
    fLUT.resize(n);
    for (size_t i = 0; i < n; ++i)
        fLUT[i] = f(MathFunctions<FloatType>::lerp(a, b, i / maxIdx));
}

// "How can I avoid linker errors with my template classes?" 
//https://isocpp.org/wiki/faq/templates#separate-template-fn-defn-from-decl
template class LookupTable<float>;
template class LookupTable<double>;

} // namespace VA
