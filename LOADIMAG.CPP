#include "stdafx.h"
#include <malloc.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "MemoryAllocate.hpp"
#include "frac.h"
#include "rtwin.hpp"
#include "png/png.h"
#include "FracWindowFunctions.hpp"
#include "FracProgress.h"

static BOOL load_bmp(RTVALS &rt, wchar_t *filename, FracProgress* progress);
static BOOL load_png(RTVALS &rt, wchar_t *filename, FracProgress* progress);

int loadimage(RTVALS &rt, const wchar_t *filename)
{
    wchar_t directory[CCHMAXPATH + 100];

    if (filename == NULL)
    {
        wcscpy_s(directory, CCHMAXPATH + 100, rt.sOpenFile);
        if (!FileOpenDialog(rt.hwndWindow, directory, CCHMAXPATH + 100))
        {
            return 0;
        }
    }
    else
    {
        wcscpy_s(directory, CCHMAXPATH + 100, filename);
    }
    int result;
    FracProgress progress((CWnd*)(rt.hwndWindow), rt, directory, false, result);
    progress.DoModal();
    return result;
}

int loadimage(RTVALS &rt, const wchar_t * filename, FracProgress* progress)
{
   wchar_t * extension;

   wchar_t directory[CCHMAXPATH + 100];
   wcscpy_s(directory, CCHMAXPATH + 100, filename);

   extension = directory + wcslen(directory);
   while ( (extension > directory) && (*extension != '.') )
   {
      --extension;
   }
   if ( extension == directory )
   {
      extension = directory + wcslen(directory);
      swprintf_s( extension, (CCHMAXPATH+2-(extension-directory)), L".pos" );
   }

   if ( !LoadPositionInfo( directory, rt, FALSE) )
   {
      DoMessageBox(rt.hwndWindow, L"Unable to open position file", L"Invalid File" );
      return 0;
   }
   wcscpy_s( rt.sOpenFile, CCHMAXPATH+2, directory );

   rt.previousxc = rt.xc;
   rt.previousyc = rt.yc;
   rt.previouszoom = rt.zoom;
   rt.previousorder = rt.order;

   swprintf_s(extension, (CCHMAXPATH + 2 - (extension - directory)), L"Counts.PNG");
   if (load_png(rt, directory, progress))
   {
       rt.paintBmpWidth = rt.bmpWidth;
       rt.paintBmpHeight = rt.bmpHeight;
       return 2;
   }
   swprintf_s(extension, (CCHMAXPATH+2-(extension-directory)), L".bmp");
   if ( load_bmp( rt, directory, progress ) )
   {
      rt.paintBmpWidth = rt.bmpWidth;
      rt.paintBmpHeight = rt.bmpHeight;
      return 1;
   }
   swprintf_s(extension, (CCHMAXPATH+2-(extension-directory)), L".png");
   if ( load_png( rt, directory, progress ) )
   {
      rt.paintBmpWidth = rt.bmpWidth;
      rt.paintBmpHeight = rt.bmpHeight;
      return 1;
   }
   DoMessageBox(rt.hwndWindow, L"Unable to read image from bitmap file", L"Invalid File" );
   return 0;
}

BOOL load_bmp(RTVALS &rt, wchar_t *filename, FracProgress* progress)
{
   FILE *fp=NULL;
   _wfopen_s(&fp,filename,L"rb");
   if ( fp == NULL )
   {
      return FALSE;
   }
   BITMAPFILEHEADER2 bfh2;
   fread(&bfh2,sizeof(bfh2),1,fp);
   if ( (bfh2.usType != 0x4d42) || (bfh2.cbSize!=sizeof(bfh2)) || (bfh2.offBits!=sizeof(bfh2))
      || (bfh2.bmp2.cbFix != sizeof(bfh2.bmp2)) || (bfh2.bmp2.cPlanes != 1) || (bfh2.bmp2.cBitCount != 24) )
   {
      return FALSE;
   }

   PVOID mem;
   if ( (rt.bmpWidth != (long)bfh2.bmp2.cx) || (rt.bmpHeight != (long)bfh2.bmp2.cy) )
   {
      rt.paintBmpWidth = rt.bmpWidth = bfh2.bmp2.cx;
      rt.paintBmpHeight = rt.bmpHeight = bfh2.bmp2.cy;
      if ( rt.bmp )
      {
         FreeMemory( rt.bmp );
      }
      rt.bmp = NULL;
      mem = AllocateBitmapMemory( rt.bmpWidth, rt.bmpHeight+1, rt.bmpPixelStep, &(rt.bmpRowSize) );
      if ( mem == NULL )
         return FALSE;
      rt.bmp=(UCHAR*)mem;
   }

   long w;
   w=((bfh2.bmp2.cBitCount*bfh2.bmp2.cx+31)/32)*bfh2.bmp2.cPlanes*4;

   if ( rt.bmpPixelStep == 3 )
   {
      fread(rt.bmp,w*bfh2.bmp2.cy,1,fp);
   }
   else
   {
      unsigned char *bmp24;
      mem = AllocateMemory(w * bfh2.bmp2.cy);
      bmp24 = (unsigned char *)mem;
      unsigned char *temp2 = rt.bmp;
      fread(bmp24,w*bfh2.bmp2.cy,1,fp);

      int x, y;
      for ( y = 0; y < (long)bfh2.bmp2.cy; y++ )
      {
         PostMessage(progress->GetSafeHwnd(), WM_IOPROGRESS, y, bfh2.bmp2.cy);
         unsigned char * temp1 = bmp24 + y * w;
         for ( x = 0; x < (long)bfh2.bmp2.cx; x++ )
         {
            temp2[0] = temp1[0];
            temp2[1] = temp1[1];
            temp2[2] = temp1[2];
            temp2+=4;
            temp1+=3;
         }
      }
      FreeMemory(bmp24);
   }

   fclose(fp);

   PostMessage(progress->GetSafeHwnd(), WM_IOPROGRESS, 100, 100);
   return TRUE;
}

BOOL load_png(RTVALS &rt, wchar_t *filename, FracProgress* progress)
{
   png_structp png_ptr;
   png_infop info_ptr;
   png_uint_32 width, height;
   int bit_depth, color_type, interlace_type;
   FILE *fp=NULL;

   _wfopen_s(&fp,filename,L"rb");
   if (fp == NULL)
      return FALSE;

   png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, png_voidp_NULL,
      png_error_ptr_NULL, png_error_ptr_NULL);
   if (png_ptr == NULL)
   {
      fclose(fp);
      return FALSE;
   }
   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL)
   {
      fclose(fp);
      png_destroy_read_struct(&png_ptr, png_infopp_NULL, png_infopp_NULL);
      return FALSE;
   }
   if (setjmp(png_jmpbuf(png_ptr)))
   {
      fclose(fp);
      png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
      return FALSE;
   }
   png_init_io(png_ptr, fp);

   png_read_info(png_ptr, info_ptr);

   png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
       &interlace_type, int_p_NULL, int_p_NULL);

   PVOID mem;
   if ( (rt.bmpWidth != (int)width) || (rt.bmpHeight != (int)height) )
   {
      rt.paintBmpWidth = rt.bmpWidth = width;
      rt.paintBmpHeight = rt.bmpHeight = height;
      if ( rt.bmp )
      {
         FreeMemory( rt.bmp );
      }
      rt.bmp = NULL;
      mem = AllocateBitmapMemory( rt.bmpWidth, rt.bmpHeight+1, rt.bmpPixelStep, &(rt.bmpRowSize) );
      if ( mem == NULL )
         return FALSE;
      rt.bmp=(UCHAR*)mem;
   }

   png_set_strip_16(png_ptr);
   png_set_bgr(png_ptr);
   if (color_type == PNG_COLOR_TYPE_PALETTE)
   {
       png_set_palette_to_rgb(png_ptr);
       color_type = PNG_COLOR_TYPE_RGB;
   }
   if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
   {
       png_set_gray_1_2_4_to_8(png_ptr);
       color_type = PNG_COLOR_TYPE_RGB;
   }

   png_bytep *row_pointers = new png_bytep[height];

   int row;
   for (row = 0; row < (int)height; row++)
   {
      row_pointers[row] = rt.bmp + (height-1-row) * rt.bmpRowSize;
   }

   png_read_image(png_ptr, row_pointers);

   if (color_type == PNG_COLOR_TYPE_RGB)
   {
      int x, y;
      for ( y = 0; y < (int)height; y++ )
      {
         PostMessage(progress->GetSafeHwnd(), WM_IOPROGRESS, y, height);
         unsigned char * temp1 = row_pointers[y] + (width-1) * 3;
         unsigned char * temp2 = row_pointers[y] + (width-1) * 4;
         for ( x = (width-1); x > 0; --x )
         {
            temp2[2] = temp1[2];
            temp2[1] = temp1[1];
            temp2[0] = temp1[0];
            temp2[3] = 255;
            temp2-=4;
            temp1-=3;
         }
         temp2[3] = 255;
      }
   }
   delete [] row_pointers;

   png_read_end(png_ptr, info_ptr);
   png_destroy_read_struct(&png_ptr, &info_ptr, png_infopp_NULL);
   fclose(fp);

   PostMessage(progress->GetSafeHwnd(), WM_IOPROGRESS, 100, 100);
   return TRUE;
}
