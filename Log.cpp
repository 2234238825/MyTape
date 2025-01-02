//
// Created by Admin on 2024/10/28.
//
#include "Log.h"
#define FATAL_LEVEL
#define ERROR_LEVEL
#define WARNING_LEVEL
#define INFO_LEVEL
#define DEBUG_LEVEL
#include "kfcStruct.h"
void Log( int  iLevel, const char *Str, ... )
{
    va_list        Start;
    va_start(Start, Str);
    char szLogString[4096];
    vsprintf(szLogString,Str,Start);
    cout<<szLogString<<endl;
}