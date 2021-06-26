#### 词法分析

词法分析是计算机科学中将字符序列转换为标记（token）序列的过程。在词法分析阶段，编译器读入 源程序字符串流，将字符流转换为标记序列，同时将所需要的信息存储，然后将结果交给语法分析器。

##### Lex

CsGo编译器的词法分析使用Lex（Flex）完成，Lex是一个产生词法分析器的程序，是大多数UNIX系统的词法分析器产生程序。 Lex读入lex文件中定义的词法分析规则，输出C语言词法分析器源码。 标准lex文件由三部分组成，分别是定义区、规则区和用户子过程区。在定义区，用户可以编写C语言中的声明语句，导入需要的头文件或声明变量。在规则区，用户需要编写以正则表达式和对应的动作的形式的代码。在用户子过程区，用户可以定义函数。

##### 正则表达式

正则表达式是通过单个字符串描述，匹配一系列符合某个句法规则的字符串。在实际应用中，常用到的语法规则如下：

|   字符   |                             描述                             |
| :------: | :----------------------------------------------------------: |
|   `\`    | 将下一个字符标记为一个特殊字符（File Format Escape，清单见本表）、或一个原义字符（Identity Escape，有^$()*+?.[\{\|共计12个)、或一个向后引用（backreferences）、或一个八进制转义符。例如，“`n`”匹配字符“`n`”。“`\n`”匹配一个换行符。序列“`\\`”匹配“`\`”而“`\(`”则匹配“`(`”。 |
|   `^`    | 匹配输入字符串的开始位置。如果设置了RegExp对象的Multiline属性，^也匹配“`\n`”或“`\r`”之后的位置。 |
|   `$`    | 匹配输入字符串的结束位置。如果设置了RegExp对象的Multiline属性，$也匹配“`\n`”或“`\r`”之前的位置。 |
|   `*`    | 匹配前面的子表达式零次或多次。例如，zo*能匹配“`z`”、“`zo`”以及“`zoo`”。*等价于{0,}。 |
|   `+`    | 匹配前面的子表达式一次或多次。例如，“`zo+`”能匹配“`zo`”以及“`zoo`”，但不能匹配“`z`”。+等价于{1,}。 |
|   `?`    | 匹配前面的子表达式零次或一次。例如，“`do(es)?`”可以匹配“`does`”中的“`do`”和“`does`”。?等价于{0,1}。 |
|  `{n}`   | *n*是一个非负整数。匹配确定的*n*次。例如，“`o{2}`”不能匹配“`Bob`”中的“`o`”，但是能匹配“`food`”中的两个o。 |
|  `{n,}`  | *n*是一个非负整数。至少匹配*n*次。例如，“`o{2,}`”不能匹配“`Bob`”中的“`o`”，但能匹配“`foooood`”中的所有o。“`o{1,}`”等价于“`o+`”。“`o{0,}`”则等价于“`o*`”。 |
| `{n,m}`  | *m*和*n*均为非负整数，其中*n*<=*m*。最少匹配*n*次且最多匹配*m*次。例如，“`o{1,3}`”将匹配“`fooooood`”中的前三个o。“`o{0,1}`”等价于“`o?`”。请注意在逗号和两个数之间不能有空格。 |
|   `?`    | 非贪心量化（Non-greedy quantifiers）：当该字符紧跟在任何一个其他重复修饰符（*,+,?，{*n*}，{*n*,}，{*n*,*m*}）后面时，匹配模式是**非**贪婪的。非贪婪模式尽可能少的匹配所搜索的字符串，而默认的贪婪模式则尽可能多的匹配所搜索的字符串。例如，对于字符串“`oooo`”，“`o+?`”将匹配单个“`o`”，而“`o+`”将匹配所有“`o`”。 |
|   `.`    | 匹配除“`\r`”“`\n`”之外的任何单个字符。要匹配包括“`\r`”“`\n`”在内的任何字符，请使用像“`(.|\r|\n)`”的模式。 |
|  `x|y`   | 没有包围在()里，其范围是整个正则表达式。例如，“`z|food`”能匹配“`z`”或“`food`”。“`(?:z|f)ood`”则匹配“`zood`”或“`food`”。 |
| `[xyz]`  | 字符集合（character class）。匹配所包含的任意一个字符。例如，“`[abc]`”可以匹配“`plain`”中的“`a`”。特殊字符仅有反斜线\保持特殊含义，用于转义字符。其它特殊字符如星号、加号、各种括号等均作为普通字符。脱字符^如果出现在首位则表示负值字符集合；如果出现在字符串中间就仅作为普通字符。连字符 - 如果出现在字符串中间表示字符范围描述；如果如果出现在首位（或末尾）则仅作为普通字符。右方括号应转义出现，也可以作为首位字符出现。 |
| `[^xyz]` | 排除型字符集合（negated character classes）。匹配未列出的任意字符。例如，“`[^abc]`”可以匹配“`plain`”中的“`plin`”。 |
| `[a-z]`  | 字符范围。匹配指定范围内的任意字符。例如，“`[a-z]`”可以匹配“`a`”到“`z`”范围内的任意小写字母字符。 |
| `[^a-z]` | 排除型的字符范围。匹配任何不在指定范围内的任意字符。例如，“`[^a-z]`”可以匹配任何不在“`a`”到“`z`”范围内的任意字符。 |
|   `\d`   | 匹配一个数字字符。等价于[0-9]。注意Unicode正则表达式会匹配全角数字字符。 |
|   `\D`   |              匹配一个非数字字符。等价于[^0-9]。              |
|   `\n`   |              匹配一个换行符。等价于\x0a和\cJ。               |
|   `\r`   |              匹配一个回车符。等价于\x0d和\cM。               |
|   `\s`   | 匹配任何空白字符，包括空格、制表符、换页符等等。等价于[ \f\n\r\t\v]。注意Unicode正则表达式会匹配全角空格符。 |
|   `\S`   |          匹配任何非空白字符。等价于[^ \f\n\r\t\v]。          |
|   `\w`   | 匹配包括下划线的任何单词字符。等价于“`[A-Za-z0-9_]`”。注意Unicode正则表达式会匹配中文字符。 |
|   `\W`   |        匹配任何非单词字符。等价于“`[^A-Za-z0-9_]`”。         |

##### 具体实现

定义区：

CsGo在定义区导入了需要的头文件，如`<stdio.h>`, `<string>`, `<sstream>`等。同时，引入了两个函数`clean_string`和`clean_char`。`clean_string`的作用是去除字符串两侧的`"`，同时处理字符串中的转义字符，使其从单纯的字符串变为真正的转义字符（如，将"\n"变为换行）。`clean_char`的作用是去除字符两侧的`'`，同时将字符串状态的转义字符变为真正的转义字符。

~~~C
#include <stdio.h>
#include <string>
#include <cstring>
#include <sstream>
#include "ast.h"
#include "parser.hpp"
extern "C" int yywrap() {return 1;}
int clean_string () {
    char* mystr;
    mystr = strdup(yytext+1) ; // copy yytext and remove leading "
    if (!mystr) return 1;
    mystr[yyleng-2] = '\0';    // remove trailing "
    for (int i = 0, j = 0; i <= strlen(mystr); i++, j++) { // "<=" and not "<" to get /0, i : mystr indice and j : yytext indice
        if (mystr[i] == '\\') {
            i++;
            if (mystr[i] == 'n')        yytext[j]='\n';
            else if (mystr[i] == 'r')   yytext[j]='\r';
            else if (mystr[i] == 't')   yytext[j]='\t';
            else if (mystr[i] == '0')   yytext[j]='\0';
            else yytext[j] = mystr[i];
        }
        else yytext[j] = mystr[i];
    }
    yyleng=strlen(yytext);
    free(mystr);
    return 0 ;
}

int clean_char () {
    printf("yytext is %s\n", yytext);
    char* mystr;
    mystr = strdup(yytext) ; // copy yytext
    if (!mystr) return 1;
    // mystr[yyleng-2] = '\0';    // remove trailing "
    for (int i = 0, j = 0; i <= strlen(mystr); i++, j++) { // "<=" and not "<" to get /0, i : mystr indice and j : yytext indice
        if (mystr[i] == '\\') {
            i++;
            if (mystr[i] == 'n')        yytext[j]='\n';
            else if (mystr[i] == 'r')   yytext[j]='\r';
            else if (mystr[i] == 't')   yytext[j]='\t';
            else if (mystr[i] == '0')   i--;
            else yytext[j] = mystr[i];
        }
        else yytext[j] = mystr[i];
    }
    yyleng=3;
    free(mystr);
    printf("yytext is %s\n", yytext);
    return 0 ;
}
~~~

规则区：

首先，排除空格、换行和回车的干扰，将它们解析成：

~~~python
[ \t\n]                             { ; }
~~~

之后，解析关键字、运算符和界符。

~~~C
[ \t\n]                             { ; }
"("                                 {return LP; }
")"                                 {return RP; }
"["                                 {return LB; }
"]"                                 {return RB; }
"{"                                 {return LCP; }
"}"                                 {return RCP; }
"."                                 {return DOT; }
","                                 {return COMMA; }
":"                                 {return COLON; }
"_"                                 {return UNDERSCORE; }
"*"                                 {return MUL; }
"/"                                 {return DIV; }
"+"                                 {return PLUS; }
"-"                                 {return MINUS; }
"%"                                 {return MOD; }
"!="                                {return UNEQUAL; }
"=="                                {return EQUAL; }
">="                                {return GE; }
"<="                                {return LE; }
">"                                 {return GT; }
"<"                                 {return LT; }
"="                                 {return ASSIGN; }
";"                                 {return SEMI; }
"&&"                                {return AND; }
"||"                                {return OR; }
"!"                                 {return NOT; }
"if"                                {return IF; }
"else"                              {return ELSE; }
"while"                             {return WHILE; }
"func"                              {return FUNC; }
"return"                            {return RETURN; }
"void"                              {
                                        yylval.sVal = new std::string(yytext, yyleng);
                                        return VOID; 
                                    }
"int"|"float"|"char"|"string"       {
                                        yylval.sVal = new std::string(yytext, yyleng);
                                        return SYS_TYPE;
                                    }
~~~

标识符是以字母或下划线开头，由字母、下划线和数字组成的字符串。

~~~C
[a-zA-Z_][a-zA-Z0-9_]*              { 
                                        yylval.sVal = new std::string(yytext, yyleng);
                                        return IDENTIFIER; 
                                    } 
~~~

整数类型`int`只支持正数。

~~~C
(0|[1-9][0-9]*)                     {
                                        int itmp;
                                        sscanf(yytext, "%d", &itmp);
                                        yylval.iVal = itmp; 
                                        return INTEGER; 
                                    }
~~~

浮点数类型`float`支持科学计数法。

~~~c
([0-9]+\.[0-9]*)([eE][-+]?[0-9]+)?  { 
                                        float ftmp;
                                        sscanf(yytext, "%f", &ftmp);
                                        yylval.fVal = ftmp;
                                        return FLOAT; 
                                    }
~~~

字符类型`char`和字符串类型`string`都支持转义字符:

~~~c
\"(\\.|[^"])*\"                     {
                                        clean_string();
                                        yylval.sVal = new std::string(yytext, yyleng);
                                        return STRING;
                                    }

[\']([^\n\t]|[\\]n|[\\]t|[\\]0|[\\]r)[\'] {
                                        char ctmp;
                                        if (yyleng == 4) {
                                            clean_char();
                                        }
                                        if (strcmp(yytext, "\'\\0\'") == 0) {
                                            ctmp = '\0';
                                            printf("ctmp = 0!!!!!!!\n");
                                        }
                                        else {
                                            sscanf(yytext, "\'%c\'", &ctmp);
                                        }
                                        yylval.cVal = ctmp;
                                        return CHAR;
                                    }
~~~



