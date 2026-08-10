#ifndef CONSTS_H
#define CONSTS_H

#include <stdint.h>

/*
Butter变量的类型枚举。
*/
enum Types {
	EndArg = -3, End, Start,
    Int, Uint, Float, String, Bool, Char,
	Array, Map, Object,
};

struct string {
	char* val;
	uint64_t len;
};

struct File {
	char*** word;
	int* lines;
	int* tokenCnt;
	int totalLines;
};


struct op {
	char* name;
	int priority;
};

/*
一个Butter对象。
value的意义根据不同的类型而不同。以下类型取value.u。
- Operator: Butter的运算符，value存储运算符在Operators数组中的索引。Operators应当是一个const数组。
- Builtin: Butter的内置函数，value存储内置函数在Builtins数组中的索引。Builtins应当是一个const数组。
- Keyword: Butter的关键字，value存储关键字在Keywords数组中的索引。Keywords应当是一个const数组。
- Function: 每个定义出来的Butter函数会根据定义的顺序存放在funcArray数组中，value存储函数在数组中的索引。
- Object: 表示Butter的类型。包含Types中的所有类型，value的值即为Types中的值。
- Variable: value存储变量在变量表中的索引。
- GVariable: value存储全局变量在全局变量表中的索引。
 */
struct ButterVariants {
    union {
		int64_t i;
    	uint64_t u;
    	double f;
    	struct string s;
    	_Bool b;
    	char c;
    } value;
	_Bool isOp;
};

/*
Butter函数类型。
vars[]表示函数的所有变量。Types应当是正常的Type，而非Variable, Function等。
code[]表示函数对应的字节码。就是原始的ButterVariants。
doRun表示是否在程序开始时运行。
 */
struct Functions {
	struct Variables* initialVars;
	int varSize;
	struct ButterVariants** code;
	int codeSize;
	_Bool doRun;
	enum Types* returnType;
	int returnSize;
};

struct Variables {
	char* name;
	enum Types type;
};
// 程序运行栈。
extern struct ButterVariants* execStack;

// 定义出来的Butter函数表。
extern struct Functions* functions;
extern int funcSize;

// 全局变量表。
extern struct ButterVariants* GlobalVars;
extern int GlobalVarsSize;

// 运算符们。
extern const char* ops[];
extern const int opSize;

// 内置函数们。
extern const char* builtins[];
extern const int builtinSize;

// 关键字们。
extern const char* keywords[];
extern const int keywordSize;

/*
 下面这些是编译时以字符串确定函数、变量、全局变量的表。编译完就丢了。
 */
extern int ntvSize;
extern char** nameToVar;
extern enum Types* ntvType;

extern int ntfSize;
extern char** nameToFunc;

extern int ntgSize;
extern char** nameToGVar;
extern enum Types* ntgType;
#endif