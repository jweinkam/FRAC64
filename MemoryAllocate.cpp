#include <stdafx.h>
#include "MemoryAllocate.hpp"

void * AllocateBitmapMemory( long width, long height, long pixelStep, long *rowSize )
{
    *rowSize = ((width * pixelStep) + 3) / 4;
    *rowSize *= 4;
    void * mem = AllocateMemory( ((ULONG)(*rowSize)) * height );
    memset( mem, 0xFF, ((ULONG)(*rowSize)) * height );
    return mem;
}

void * AllocateMemory( ULONG size )
{
    return (void*)(new char[size]); 
}

void FreeMemory( void * ptr )
{
    delete [] (char*)ptr;
}
