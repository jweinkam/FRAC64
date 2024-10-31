#ifndef MEMORYALLOCATE_HPP
#define MEMORYALLOCATE_HPP

void * AllocateBitmapMemory( long width, long height, long pixelStep, long *rowSize );
void * AllocateMemory( ULONG size );
void FreeMemory( void * ptr );

#endif
