//
// Created by liuli on 2026/7/27.
//
#include "compile.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "functions.h"
// 检查字符串是否只包含空白字符（空格、制表符、换行等）
int is_blank_line(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isspace((unsigned char)str[i])) {
            return 0;  // 发现非空白字符，不是空行
        }
    }
    return 1;  // 全是空白字符
}

int opCompare (const struct op* a, const struct op* b) {
    return strcmp(a->name, b->name);
}

uint64_t opHash (const struct op* a, uint64_t seed1, uint64_t seed2) {
    return hashmap_sip(a->name, strlen(a->name), seed1, seed2);
}


// 判断是否是运算符字符
int is_operator_char(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' ||
           c == '=' || c == '!' || c == '<' || c == '>' ||
           c == '&' || c == '|' || c == '^' || c == '%' ||
           c == '~';
}

// 获取多字符运算符的长度
int get_multi_char_operator_len(const char *str, int pos) {
    int len = strlen(str);
    int max_len = 0;

    // 检查所有可能的多字符运算符
    if (pos + 1 < len) {
        char two[3] = {str[pos], str[pos+1], '\0'};

        // 比较运算符
        if (strcmp(two, "==") == 0 || strcmp(two, "!=") == 0 ||
            strcmp(two, "<=") == 0 || strcmp(two, ">=") == 0 ||
            strcmp(two, "&&") == 0 || strcmp(two, "||") == 0 ||
            strcmp(two, "+=") == 0 || strcmp(two, "-=") == 0 ||
            strcmp(two, "*=") == 0 || strcmp(two, "/=") == 0 ||
            strcmp(two, "%=") == 0 || strcmp(two, "->") == 0) {
            max_len = 2;
        }
    }



    if (max_len == 0 && is_operator_char(str[pos])) {
        max_len = 1;
    }

    return max_len;
}

// 分割一行，返回 token 数组
char** split_line(const char *line, int *token_count, int *error) {
    char **tokens = NULL;
    *token_count = 0;
    *error = 0;

    int len = strlen(line);
    int i = 0;

    while (i < len) {
        // 跳过空格
        while (i < len && isspace(line[i])) {
            i++;
        }
        if (i >= len) break;

        // 检查是否是括号
        if (line[i] == '(' || line[i] == ')' ||
            line[i] == '[' || line[i] == ']' ||
            line[i] == '{' || line[i] == '}') {
            char** tempToken = realloc(tokens, (*token_count + 1) * sizeof(char*));
            tokens = tempToken;
            tokens[*token_count] = malloc(2 * sizeof(char));
            tokens[*token_count][0] = line[i];
            tokens[*token_count][1] = '\0';
            (*token_count)++;
            i++;
            continue;
        }

        // 检查是否是运算符
        if (is_operator_char(line[i])) {
            int op_len = get_multi_char_operator_len(line, i);
            char** tempToken = realloc(tokens, (*token_count + 1) * sizeof(char*));
            tokens = tempToken;
            tokens[*token_count] = malloc((op_len + 1) * sizeof(char));
            strncpy(tokens[*token_count], line + i, op_len);
            tokens[*token_count][op_len] = '\0';
            (*token_count)++;
            i += op_len;
            continue;
        }

        // 检查是否以引号开头
        if (line[i] == '"' || line[i] == '\'') {
            char quote_char = line[i];
            i++;
            int start = i;
            while (i < len && line[i] != quote_char) {
                if (line[i] == '\\' && i + 1 < len) {
                    i += 2;
                } else {
                    i++;
                }
            }
            if (i >= len) {
                *error = 1;
                for (int j = 0; j < *token_count; j++) free(tokens[j]);
                free(tokens);
                return NULL;
            }
            int content_len = i - start;
            char** tempToken = realloc(tokens, (*token_count + 1) * sizeof(char*));
            tokens = tempToken;
            tokens[*token_count] = malloc((content_len + 1) * sizeof(char));
            strncpy(tokens[*token_count], line + start, content_len);
            tokens[*token_count][content_len] = '\0';
            (*token_count)++;
            i++;
            continue;
        }

        // 普通 token
        int start = i;
        while (i < len && !isspace(line[i]) &&
               line[i] != '"' && line[i] != '\'' &&
               line[i] != '(' && line[i] != ')' &&
               line[i] != '[' && line[i] != ']' &&
               line[i] != '{' && line[i] != '}' &&
               !is_operator_char(line[i])) {
            i++;
        }
        int content_len = i - start;
        if (content_len > 0) {
            char** tempToken = realloc(tokens, (*token_count + 1) * sizeof(char*));
            tokens = tempToken;
            tokens[*token_count] = malloc((content_len + 1) * sizeof(char));
            strncpy(tokens[*token_count], line + start, content_len);
            tokens[*token_count][content_len] = '\0';
            (*token_count)++;
        }
    }

    return tokens;
}

bool isValidVariableName(const char *name) {
    // 检查是否为空
    if (name == NULL || strlen(name) == 0) {
        return false;
    }

    // 检查第一个字符：必须是字母或下划线
    if (!isalpha(name[0]) && name[0] != '_') {
        return false;
    }

    // 检查后续字符：只能是字母、数字或下划线
    for (int i = 1; name[i] != '\0'; i++) {
        if (!isalnum(name[i]) && name[i] != '_') {
            return false;
        }
    }

    return true;
}

struct File* readFile(const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("failed: %s\n", filename);
        return NULL;
    }

    struct File *file = malloc(sizeof(struct File));
    if (!file) {
        fclose(fp);
        return NULL;
    }

    char buffer[1024];
    int validLines = 0;
    while (fgets(buffer, sizeof(buffer), fp)) {
        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r')) {
            buffer[len-1] = '\0';
            if (len > 1 && buffer[len-2] == '\r') {
                buffer[len-2] = '\0';
            }
        }
        if (!is_blank_line(buffer)) {
            validLines++;
        }
    }
    rewind(fp);

    file->word = malloc(validLines * sizeof(char**));
    if (!file->word) {
        free(file);
        fclose(fp);
        return NULL;
    }

    file->tokenCnt = malloc(validLines * sizeof(int));
    if (!file->tokenCnt) {
        free(file->word);
        free(file);
        fclose(fp);
        return NULL;
    }

    file->totalLines = validLines;

    int row = 0;
    int currentLine = 0;
    file->lines = NULL;

    while (fgets(buffer, sizeof(buffer), fp) && row < validLines) {
        currentLine++;

        size_t len = strlen(buffer);
        if (len > 0 && (buffer[len-1] == '\n' || buffer[len-1] == '\r')) {
            buffer[len-1] = '\0';
            if (len > 1 && buffer[len-2] == '\r') {
                buffer[len-2] = '\0';
            }
        }

        if (is_blank_line(buffer)) {
            continue;
        }

        int token_count = 0;
        int error = 0;
        char **tokens = split_line(buffer, &token_count, &error);

        if (error) {
            NewError("unmatched quotation mark", filename, currentLine);
        }

        file->word[row] = malloc((token_count + 1) * sizeof(char*));  // +1 留 NULL 结尾
        if (!file->word[row]) {
            for (int r = 0; r < row; r++) {
                for (int c = 0; c < file->tokenCnt[r]; c++) {
                    free(file->word[r][c]);
                }
                free(file->word[r]);
            }
            free(file->word);
            free(file->tokenCnt);
            free(file);
            fclose(fp);
            return NULL;
        }

        for (int col = 0; col < token_count; col++) {

            file->word[row][col] = malloc((strlen(tokens[col]) + 1) * sizeof(char));
            if (!file->word[row][col]) {
                return NULL;
            }
            strcpy(file->word[row][col], tokens[col]);
            free(tokens[col]);
        }
        file->word[row][token_count] = NULL;


        int* tempLines = realloc(file->lines,(row+1) * sizeof(int));

        if (!tempLines) {
            free(file->word[row]);
            free(file->tokenCnt);
            free(file);
            fclose(fp);
            return NULL;
        }
        file->lines = tempLines;
        file->lines[row] = currentLine;


        file->tokenCnt[row] = token_count;
        free(tokens);
        row++;
    }

    fclose(fp);

    return file;
}


struct ButterVariants* compileFile(struct File* file, char* filename) {
    // 第一遍读取：获取所有函数和全局变量信息
    int hugeQuotMark = 0;
    char* token;
    char*** code = file->word;
    int* codeLen = file->tokenCnt;
    int lineCnt = file->totalLines;



    for (int i = 0; i < lineCnt; i++) { // 第一遍读取：获取全局变量和函数信息

        char** row = code[i];


        if (row[0] == "global") {        // 如果是全局变量
            if (hugeQuotMark) {
                NewError("declaration of global variable inside function", filename, i + 1);
            }
            int startKey = ntgSize;
            int cIndex = 0;
            for (int j = 1; j < codeLen[i]; j++) {
                if (row[j] != "int" || row[j] != "float" || row[j] != "string" || row[j] != "char"
                    || row[j] != "uint" || row[j] != "bool") {
                    if (!isValidVariableName(row[j])) {
                        NewError("invalid variable name", filename, i + 1);
                    }
                    ntgSize++;
                    char** nameToGVar_temp = realloc(nameToGVar, ntgSize * sizeof(char*));
                    if (!nameToGVar_temp) {
                        free(nameToGVar);
                    }
                    nameToGVar = nameToGVar_temp;
                    nameToGVar[ntgSize-1] = row[j];
                } else {
                    enum Types* ntgType_temp = realloc(ntgType, ntgSize * sizeof(int));
                    if (!ntgType_temp) {
                        free(ntgType);
                    }
                    ntgType = ntgType_temp;
                    for (int k = startKey; k < ntgSize; k++) {
                        if (row[j] == "int") {
                            ntgType[k] = Int;
                        } else if (row[j] == "float") {
                            ntgType[k] = Float;
                        } else if (row[j] == "string") {
                            ntgType[k] = String;
                        } else if (row[j] == "char") {
                            ntgType[k] = Char;
                        } else if (row[j] == "uint") {
                            ntgType[k] = Uint;
                        } else if (row[j] == "bool") {
                            ntgType[k] = Bool;
                        }
                    }
                    break;
                }
                cIndex++;
            }
            if (cIndex != codeLen[i] + 2) {
                if (codeLen[i] - cIndex - 3 != cIndex) {
                    NewError("unmatched variable count and value count", filename, i + 1);
                }
                for (int j = cIndex + 3; j < codeLen[i]; j++) {
                    GlobalVarsSize++;
                    struct ButterVariants* GlobalVars_temp = realloc(GlobalVars, GlobalVarsSize * sizeof(struct ButterVariants));
                    if (!GlobalVars_temp) {
                        free(GlobalVars);
                    }
                    if (stot(row[j]) != ntgType[ntgSize - cIndex + j - cIndex - 3]) {
                        NewError("unmatched type", filename, i + 1);
                    }
                    struct ButterVariants value = stob(row[j], filename, i+1);
                    GlobalVars = GlobalVars_temp;
                    GlobalVars[GlobalVarsSize-1] = value;
                }
            }
        } else if (row[0] == "func" || row[0] == "run") {  // 如果是函数
            int startKey = ntfSize;
            int j = 1;
            int inQuotes = 0;
            struct Functions bFunc;
            bFunc.doRun = row[0] == "run";
            bFunc.varSize = 0;
            bFunc.initialVars = NULL;
            bFunc.returnSize = 0;
            bFunc.returnType = NULL;
            while (row[j] != "->") { // 获取函数名称和初始变量
                if (row[j] == "(") {
                    inQuotes++;
                } else if (row[j] == ")") {
                    inQuotes--;
                } else {
                    if (!inQuotes) {
                        if (ntfSize == startKey) {
                            ntfSize++;
                            char** nameToFunc_temp = realloc(nameToFunc, ntfSize * sizeof(char*));
                            if (!nameToFunc_temp) {
                                free(nameToFunc);
                            }
                            nameToFunc = nameToFunc_temp;
                            if (isValidVariableName(row[j])) {
                                nameToFunc[ntfSize-1] = row[j];
                            } else {
                                NewError("invalid function name", filename, i + 1);
                            }

                        } else {
                            NewError("extra function name", filename, i + 1);
                        }
                    } else {
                        struct Variables var = {};
                        if (row[j] != "int" && row[j] != "float" && row[j] != "string" &&
                            row[j] != "char" && row[j] != "uint" && row[j] != "bool") {
                            if (!isValidVariableName(row[j])) {
                                NewError("invalid variable name", filename, i + 1);
                            }
                            if (var.name == "" || var.type == -1) {
                                NewError("incomplete variable declaration", filename, i + 1);
                            }
                            var.name = "";
                            var.type = -1;
                            var.name = row[j];
                        } else if (row[i] == ",") {

                        } else {
                            if (row[j] == "int") {
                                var.type = Int;
                            } else if (row[j] == "float") {
                                var.type = Float;
                            } else if (row[j] == "string") {
                                var.type = String;
                            } else if (row[j] == "char") {
                                var.type = Char;
                            } else if (row[j] == "uint") {
                                var.type = Uint;
                            } else if (row[j] == "bool") {
                                var.type = Bool;
                            } else {
                                NewError("unknown type", filename, i + 1);
                            }
                            bFunc.varSize++;
                            struct Variables* var_temp = realloc(bFunc.initialVars, bFunc.varSize * sizeof(struct Variables));
                            if (!var_temp) {
                                free(bFunc.initialVars);
                            }
                            bFunc.initialVars = var_temp;
                            if (var.name == "" || var.type == -1) {
                                NewError("incomplete variable declaration", filename, i + 1);
                            }
                            bFunc.initialVars[bFunc.varSize-1] = var;
                        }
                    }
                }
                j++;
            }
            j++;
            while (row[j] != "{") {  // 获取返回值类型
                if (row[j] == "int") {
                    bFunc.returnSize++;
                    enum Types* temp_type = realloc(bFunc.returnType, bFunc.returnSize * sizeof(int));
                    if (!temp_type) {
                        free(bFunc.returnType);
                    }
                    bFunc.returnType = temp_type;
                    bFunc.returnType[bFunc.returnSize-1] = Int;
                } else if (row[j] == "float") {
                    bFunc.returnSize++;
                    enum Types* temp_type = realloc(bFunc.returnType, bFunc.returnSize * sizeof(int));
                    if (!temp_type) {
                        free(bFunc.returnType);
                    }
                    bFunc.returnType = temp_type;
                    bFunc.returnType[bFunc.returnSize-1] = Float;

                } else if (row[j] == "string") {
                    bFunc.returnSize++;
                    enum Types* temp_type = realloc(bFunc.returnType, bFunc.returnSize * sizeof(int));
                    if (!temp_type) {
                        free(bFunc.returnType);
                    }
                    bFunc.returnType = temp_type;
                    bFunc.returnType[bFunc.returnSize-1] = String;
                } else if (row[j] == "char") {
                    bFunc.returnSize++;
                    enum Types* temp_type = realloc(bFunc.returnType, bFunc.returnSize * sizeof(int));
                    if (!temp_type) {
                        free(bFunc.returnType);
                    }
                    bFunc.returnType = temp_type;
                    bFunc.returnType[bFunc.returnSize-1] = Char;
                } else if (row[j] == "uint") {
                    bFunc.returnSize++;
                    enum Types* temp_type = realloc(bFunc.returnType, bFunc.returnSize * sizeof(int));
                    if (!temp_type) {
                        free(bFunc.returnType);
                    }
                    bFunc.returnType = temp_type;
                    bFunc.returnType[bFunc.returnSize-1] = Uint;
                } else if (row[j] == "bool") {
                    bFunc.returnSize++;
                    enum Types* temp_type = realloc(bFunc.returnType, bFunc.returnSize * sizeof(int));
                    if (!temp_type) {
                        free(bFunc.returnType);
                    }
                    bFunc.returnType = temp_type;
                    bFunc.returnType[bFunc.returnSize-1] = Bool;
                } else if (row[j] == ",") {

                } else {
                    NewError("unknown type", filename, i + 1);
                }
                j++;
            }
        }
    }
}
