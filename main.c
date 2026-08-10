#include <stdio.h>
#include <stdint.h>
#include "functions.h"
#include "consts.h"
#include "compile.h"
#include <dirent.h>
int ntvSize = 0;
int ntgSize = 0;
int ntfSize = 0;
char** nameToVar = NULL;
char** nameToGVar = NULL;
char** nameToFunc = NULL;
int main() {
    struct File *file = readFile("test.but");
    if (!file) {
        return 1;
    }
    printf("total lines: %d\n", file->totalLines);
    for (int row = 0; row < file->totalLines; row++) {
        printf(" line %d (token count: %d): ", file->lines[row], file->tokenCnt[row]);
        for (int col = 0; col < file->tokenCnt[row]; col++) {
            printf("[%s] ", file->word[row][col]);
        }
        printf("\n");
    }

    // 清理内存
    for (int row = 0; row < file->totalLines; row++) {
        for (int col = 0; col < file->tokenCnt[row]; col++) {
            free(file->word[row][col]);
        }
        free(file->word[row]);
    }
    free(file->word);
    free(file->tokenCnt);
    free(file);

    return 0;
}