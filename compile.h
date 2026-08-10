#ifndef COMPILE_H
#define COMPILE_H

#include "hashmap.h"
#include "consts.h"

int opCompare (const struct op* a, const struct op* b);

uint64_t opHash (const struct op* a, uint64_t seed0, uint64_t seed1);

struct File* readFile(const char *filename);

struct ButterVariants* compileFile(struct File* file, char* filename);

int is_blank_line(const char* str);

int is_operator_char(char c);

int get_multi_char_operator_len(const char *str, int pos);

char** split_line(const char *line, int *token_count, int *error);

bool isValidVariableName(const char *name);
#endif