#ifndef RENDER_HPP
#define RENDER_HPP

#include "FracTemplate.hpp"

#undef PROTO
#define PROTO( a )                                \
void RemoveFrac ## a();                           \
BOOL IsFrac ## a(int size);                       \
void AddFrac ## a(int size, RTVALS *rt);          \
long RenderFrac ## a( );                          \
void LoadCounts ## a(bool CallCalcColor);         \
void RecolorBMP ## a(bool CallCalcColor);         \
long EdgePixelCount ## a()

PROTO(I);
PROTO(F);

#endif
