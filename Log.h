//
// Created by Admin on 2024/10/28.
//

#ifndef UNTITLED13_LOG_H
#define UNTITLED13_LOG_H
#include <stdio.h>
#include <stdlib.h>

#define FATAL_LEVEL 0
#define ERROR_LEVEL 1
#define WARNING_LEVEL 2
#define INFO_LEVEL 3
#define DEBUG_LEVEL 4
#include <stdarg.h>
#include <stdio.h>
void Log( int  iLevel, const char *Str, ... );

#endif //UNTITLED13_LOG_H
