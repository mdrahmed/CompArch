/*
 * Copyright (C) 2009-2021 Intel Corporation.
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <stdlib.h>
#if !defined(TARGET_WINDOWS)

#define EXPORT_SYM extern

#else //defined(TARGET_WINDOWS)

#include <windows.h>
// declare all functions as exported so pin can find them,
// must be all functions since only way to find end of one function is the begining of the next
// Another way is to compile application with debug info (Zi) - pdb file, but that causes probelms
// in the running of the script
#define EXPORT_SYM __declspec(dllexport)

#endif

extern void SetAppFlags_asm(unsigned int val);
extern void ClearAcFlag_asm();
extern int GetFlags_asm();

int main()
{
    unsigned int flags;
    SetAppFlags_asm(0x40000);
    flags = GetFlags_asm();
    ClearAcFlag_asm();
    if ((GetFlags_asm() & 0x40000) != 0)
    {
        exit(0);
    }
    SetAppFlags_asm(0x40000);
    flags = GetFlags_asm();
    ClearAcFlag_asm();
    printf("SUCCESS\n");
    exit(0);
}
