#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "consts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

_Bool inArr(int size, const char* arr[size], char* v);

_Bool inArrPtr(int size, char** arr, char* v);

_Noreturn void NewError(char* message, char* file, int line);

/*
 将一个字符串转换为Butter变量。
 顺序：
 - 字符串类型。
 - 整数类型。
 - 浮点数类型。
 - 布尔类型。
 - 运算符、内置函数、关键字
 - 已有的自定义函数。
 - 已有的局部变量。
 - 已有的全局变量。
 */
struct ButterVariants stob(char* s, char* file, int line);

enum Types stot(char* s);
#endif