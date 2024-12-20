//
// Created by Admin on 2024/10/28.
//
#include "Log.h"

void Log( int  iLevel, const char *Str, ... )
{
    va_list        Start;
    va_start(Start, Str);
    char szLogString[4096];
    sprintf(szLogString,Str,Start);
    printf("%s\n",szLogString);
}