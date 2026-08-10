#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "functions.h"
#include "consts.h"



struct ButterVariants stob(char* s, char* file, int line) {

    struct ButterVariants bVar = {0};
    char* end_ptr = NULL;
    const uint64_t len = strlen(s);

    // 转换字符串
    if (s[0] == '"' && s[len-1] == '"') {
        uint64_t content_len = len - 2;
        char* content = malloc(content_len + 1);
        if (!content) {
            NewError("out of memory", file, line);
        }
        memcpy(content, s + 1, content_len);
        content[content_len] = '\0';
        bVar.value.s.val = content;
        bVar.value.s.len = content_len;
        return bVar;
    }
    // 转换字符
    if (s[0] == '\'' && s[len-1] == '\'') {
        if (strlen(s) != 3) {
            NewError("char object must be one character", file, line);
        }
        bVar.value.c = s[1];
        return bVar;
    }
    // 转换整数
    errno = 0;
    const int64_t int_val = strtoll(s, &end_ptr, 10);
    if (end_ptr != s && *end_ptr == '\0' && errno != ERANGE) {
        bVar.value.i = int_val;
        return bVar;
    }
    // 转换浮点数
    errno = 0;
    const double float_val = strtod(s, &end_ptr);
    if (end_ptr != s && *end_ptr == '\0' && errno != ERANGE) {
        bVar.value.f = float_val;
        return bVar;
    }
    // 转换布尔值
    if (strcmp(s, "true") == 0 || strcmp(s, "True") == 0 || strcmp(s, "TRUE") == 0 ) {
        bVar.value.b = true;
        return bVar;
    }
    if (strcmp(s, "false") == 0 || strcmp(s, "False") == 0 || strcmp(s, "FALSE") == 0 ) {
        bVar.value.b = false;
        return bVar;
    }
    // 转换运算符、内置函数、关键字
    if (inArr(opSize, ops, s)) {
        uint64_t ind = 0;
        for (int i = 0; i < opSize; i++) {
            if (strcmp(ops[i], s) == 0) {
                ind = i;
                break;
            }
        }
        bVar.value.u = ind;
        return bVar;
    }
    if (inArr(builtinSize, builtins, s)) {
        uint64_t ind = 0;
        for (int i = 0; i < builtinSize; i++) {
            if (strcmp(builtins[i], s) == 0) {
                ind = i;
                break;
            }
        }
        bVar.value.u = ind;
        return bVar;
    }
    if (inArr(keywordSize, keywords, s)) {
        uint64_t ind = 0;
        for (int i = 0; i < keywordSize; i++) {
            if (strcmp(keywords[i], s) == 0) {
                ind = i;
                break;
            }
        }
        bVar.value.u = ind;
        return bVar;
    }
    // 转换已有函数。
    if (inArrPtr(ntfSize, nameToFunc, s)) {
        uint64_t ind = 0;
        for (int i = 0; i < ntfSize; i++) {
            if (strcmp(nameToFunc[i], s) == 0) {
                ind = i;
                break;
            }
        }
        bVar.value.u = ind;
        return bVar;
    }
    // 转换局部变量。
    if (inArrPtr(ntvSize, nameToVar, s)) {
        uint64_t ind = 0;
        for (int i = 0; i < ntvSize; i++) {
            if (strcmp(nameToVar[i], s) == 0) {
                ind = i;
                break;
            }
        }
        bVar.value.u = ind;
        return bVar;
    }
    // 转换全局变量。
    if (inArrPtr(ntgSize, nameToGVar, s)) {
        uint64_t ind = 0;
        for (int i = 0; i < ntgSize; i++) {
            if (strcmp(nameToGVar[i], s) == 0) {
                ind = i;
                break;
            }
        }
        bVar.value.u = ind;
        return bVar;
    }
    NewError("unknown token", file, line);
}

_Noreturn void NewError(char* message, char* file, int line) {
    printf("Error: %s\n\tat file:%s, line: %d", message, file, line);
    exit(-1);
}

_Bool inArrPtr(int size, char** arr, char* v) {
    for (int i = 0; i < size; i++) {
        if (strcmp(arr[i], v) == 0) {
            return true;
        }
    }
    return false;
}

_Bool inArr(int size, const char* arr[size], char* v) {
    for (int i = 0; i < size; i++) {
        if (strcmp(arr[i], v) == 0) {
            return true;
        }
    }
    return false;
}

enum Types stot(char* s) {
    const uint64_t len = strlen(s);
    char* end_ptr = NULL;

    if (s[0] == '"' && s[len-1] == '"') {
        return String;
    }
    if (s[0] == '\'' && s[len-1] == '\'') {
        return Char;
    }
    // 转换整数
    errno = 0;
    const int64_t int_val = strtoll(s, &end_ptr, 10);
    if (end_ptr != s && *end_ptr == '\0' && errno != ERANGE) {
        return Int;
    }
    // 转换浮点数
    errno = 0;
    const double float_val = strtod(s, &end_ptr);
    if (end_ptr != s && *end_ptr == '\0' && errno != ERANGE) {
        return Float;
    }
    // 转换布尔值
    if (strcmp(s, "true") == 0 || strcmp(s, "True") == 0 || strcmp(s, "TRUE") == 0 ) {
        return Bool;
    }
    if (strcmp(s, "false") == 0 || strcmp(s, "False") == 0 || strcmp(s, "FALSE") == 0 ) {
        return Bool;
    }

    if (inArrPtr(ntvSize, nameToVar, s)) {
        uint64_t ind = 0;
        for (int i = 0; i < ntvSize; i++) {
            if (strcmp(nameToVar[i], s) == 0) {
                ind = i;
                break;
            }
        }
        return ntvType[ind];
    }
    if (inArrPtr(ntgSize, nameToGVar, s)) {
        uint64_t ind = 0;
        for (int i = 0; i < ntgSize; i++) {
            if (strcmp(nameToGVar[i], s) == 0) {
                ind = i;
                break;
            }
        }
        return ntgType[ind];
    }

    return Object;
}