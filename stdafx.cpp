
// stdafx.cpp : source file that includes just the standard includes
// FRAC64NonTemplate2017.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

void DosCopy(const wchar_t *src, const wchar_t *dest, ULONG option)
{
	option = option;
	CopyFile(src, dest, FALSE);
}
