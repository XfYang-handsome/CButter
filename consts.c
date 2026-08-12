#include <stdio.h>
#include "consts.h"

const char* ops[] = {
    "+", "-", "*", "/", "%", "++", "--",
    "==", "!=", ">", "<", ">=", "<=",
    "&&", "||", "!",
    "&", "|", "^", "~",
    "="
};
const int opSize = sizeof(ops)/sizeof(ops[0]);

const char* builtins[] = {};
const int builtinSize = sizeof(builtins)/sizeof(builtins[0]);

const char* keywords[] = {};
const int keywordSize = sizeof(keywords)/sizeof(keywords[0]);


char** nameToGVar = NULL;
char** nameToFunc = NULL;
char** nameToVar = NULL;
enum Types* ntgType = NULL;
enum Types* ntvType = NULL;
struct ButterVariants* GlobalVars = NULL;
struct Functions* functions = NULL;

int ntgSize, ntvSize, ntfSize, funcSize, GlobalVarsSize= 0;