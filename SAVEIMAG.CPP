#include "stdafx.h"

#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "frac.h"
#include "rtwin.hpp"
#include "png/png.h"
#include "MemoryAllocate.hpp"
#include "FracWindowFunctions.hpp"
#include "FracProgress.h"

static void write_png(RTVALS &rt, wchar_t * sFilename, FracProgress *progress, bool first);

void saveimage(RTVALS &rt)
{
    wchar_t directory[CCHMAXPATH+100];
    wcscpy_s( directory, CCHMAXPATH+100, rt.sOpenFile );
    if ( FileSaveDialog( rt.hwndWindow, directory, CCHMAXPATH+100 ) )
    {
        int result;
        FracProgress progress((CWnd*)(rt.hwndWindow), rt, directory, true, result);
        progress.DoModal();
    }
}

extern void RecolorBMP(RTVALS *rt, bool CallCalcColor);
void saveimage(RTVALS &rt, const wchar_t * sFilename, FracProgress *progress, bool SaveCounts)
{
    wchar_t directory[CCHMAXPATH+2];

    wcscpy_s( directory, CCHMAXPATH+2, sFilename );

   wchar_t * extension;

   extension = directory + wcslen(directory);
   while ( (extension > directory) && (*extension != '.') )
   {
      --extension;
   }
   if (extension == directory)
   {
       extension += wcslen(directory);
   }

    swprintf_s( extension, (CCHMAXPATH+2-(extension-directory)), L".PNG" );
    write_png( rt, directory, progress, true );

    swprintf_s(extension, (CCHMAXPATH + 2 - (extension - directory)), L".POS");
    wcscpy_s(rt.sOpenFile, CCHMAXPATH + 2, directory);
    SavePositionInfo(directory, rt);

    swprintf_s(extension, (CCHMAXPATH + 2 - (extension - directory)), L".LOG");
    DosCopy(L"frac.log", directory, DCPY_EXISTING);

    if (SaveCounts)
    {
        RecolorBMP(&rt, false);
        swprintf_s(extension, (CCHMAXPATH + 2 - (extension - directory)), L"Counts.PNG");
        write_png(rt, directory, progress, false);
        RecolorBMP(&rt, true);
    }
    PostMessage(progress->GetSafeHwnd(), WM_IOPROGRESS, 100, 100);
}

void write_png(RTVALS &rt, wchar_t * sFilename, FracProgress *progress, bool first)
{
   FILE *fp=NULL;
   png_structp png_ptr;
   png_infop info_ptr;

   _wfopen_s(&fp, sFilename, L"wb");
   if (fp == NULL)
      return;

   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, png_voidp_NULL,
      png_error_ptr_NULL, png_error_ptr_NULL);
   if (png_ptr == NULL)
   {
      fclose(fp);
      return;
   }
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      fclose(fp);
      png_destroy_write_struct(&png_ptr, png_infopp_NULL);
      return;
   }
   if (setjmp(png_jmpbuf(png_ptr)))
   {
      fclose(fp);
      png_destroy_write_struct(&png_ptr, &info_ptr);
      return;
   }
   png_init_io(png_ptr, fp);

   png_set_IHDR(png_ptr, info_ptr, rt.bmpWidth, rt.bmpHeight, 8, PNG_COLOR_TYPE_RGBA,
      PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

   png_write_info(png_ptr, info_ptr);

   if ( rt.bmpPixelStep == 4 )
   {
      png_set_bgr(png_ptr);

      int y;
      for ( y = rt.bmpHeight-1; y >= 0; --y )
      {
         PostMessage(progress->GetSafeHwnd(), WM_IOPROGRESS, (first ? rt.bmpHeight : 2*rt.bmpHeight) - y, rt.bmpHeight*2);
         unsigned char * temp = rt.bmp + y * rt.bmpRowSize;
         png_write_rows(png_ptr, (png_bytepp)&temp, 1);
      }
   }
   else
   {
      unsigned char *bmp32;
      PVOID mem = AllocateMemory(rt.bmpWidth*4);
      bmp32 = (unsigned char *)mem;
      int x, y;
      for ( y = rt.bmpHeight-1; y >= 0; --y )
      {
         PostMessage(progress->GetSafeHwnd(), WM_IOPROGRESS, (first ? rt.bmpHeight : 2 * rt.bmpHeight) - y, rt.bmpHeight * 2);
         unsigned char * temp1 = bmp32;
         unsigned char * temp2 = rt.bmp + y * 3 * rt.bmpWidth;
         for ( x = 0; x < rt.bmpWidth; x++ )
         {
            temp1[2] = temp2[0];
            temp1[1] = temp2[1];
            temp1[0] = temp2[2];
            temp1[3] = 255;
            temp2+=3;
            temp1+=4;
         }
         png_write_rows(png_ptr, (png_bytepp)&bmp32, 1);
      }
      FreeMemory(bmp32);
   }

   png_write_end(png_ptr, info_ptr);

   png_destroy_write_struct(&png_ptr, &info_ptr);

   fclose(fp);
}
