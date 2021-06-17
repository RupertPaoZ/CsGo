#### 语法分析

在计算机科学和语言学中，语法分析是根据某种给定的形式文法对由单词序列（如英语单词序列）构成的输入文本进行分析并确定其语法结构的一种过程。在词法分析阶段，编译器接收词法分析器发送的标记序列，最终输出抽象语法树数据结构。

##### Yacc

CsGo编译器的语法分析使用Yacc（Bison）完成。Yacc是Unix/Linux上一个用来生成编译器的编译器（编译器代码生成器）。Yacc生成的编译器主要是用C语言写成的语法解析器（Parser），需要与词法解析器Lex一起使用，再把两部分产生出来的C程序一并编译。

与Lex相似，Yacc的输入文件由以%%分割的三部分组成，分别是声明区、规则区和程序区。三部分的功能与Lex相似，不同的是规则区的正则表达式替换为CFG，在声明区要提前声明好使用到的终结符以及非终结符的类型。

~~~C
declarations
%%
rules
%%
programs
~~~

##### 语法规则

CsGo的语法规则如下：

~~~
1. program -> declaration-list
2. declaration-list -> declaration-list declaration | declaration
3. declaration -> var-declaration | fun-declaration
4. var-declaration -> type-specifier ID; | type-specifier ID [NUM];
5. type-specifier -> int | float | void
6. fun-declaration -> func ID(params) (params) compound-stmt
7. params -> param-list | void
8. param-list -> param-list, param | param
9. param -> ID type-specifier | ID [] type-specifier 
10. compound-stmt -> {local-declarations statement-list}
11. local-declarations -> local-declarations var-declaration | empty
12. statement-list -> statement-list statement | empty
13. statement -> expression-stmt | compound-stmt | selection-stmt
                 | iteration-stmt | return-stmt | function-stmt
14. expression-stmt -> expression; | ;
15. selection-stmt -> if (simple-expression) statement
                      | if (simple-expression) statement else statement
16. iteration-stmt -> while (simple-expression) statement
17. return-stmt -> return expression; | return INTEGER; | return FLOAT;
18. function-stmt -> call;
19. expression -> var-list = expression-list
20. expression-list -> expression-list, simple-expression | simple-expression
21. var-list -> var-list, var | var | _	
22. var -> ID | ID [simple-expression]
23. simple-expression -> additive-expression relop additive_expr | additive_expr
24. relop -> <= | < | > | >= | == | !=
25. additive-expression -> additive-expression addop term | term
26. addop -> + | -
27. term -> term mulop factor | factor
28. mulop -> * | /
29. factor -> (simple-expression) | var | call | NUM
30. call -> ID (args)
31. args -> arg-list | empty
32. arg-list -> arg-list, simple-expression | simple-expression
~~~

##### 声明区

在声明区引入相关的头文件，以及定义错误处理函数。

~~~C
#include "ppMacro.h"
#include "ast.h"
#include "CodeGenerator.h"
#include "ObjGenerator.h"
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>

using namespace std;

void yyerror(const char *s) { 
    std::printf("Error: %s\n", s);
    std::exit(1); 
}

int yylex();
Program* root = nullptr;
~~~

##### 规则定义区

首先，定义非终结符对应的数据类型：

~~~C
%union {
    int iVal;
    float fVal;
    char cVal;
    std::string* sVal;
    Program *program;
    Identifier *identifier;
    SysType *systype;
    Integer *integer;
    Float *nfloat;
    Char *nchar;
    String *nstring;
    UnderScore *underscore;
    Void *nvoid;
    Decl *decl;
    VarDecl *vardecl;
    FuncDecl *funcdecl;
    TypeSpec *typespec;
    Param *param;
    Params *params;
    ComStmt *comstmt;
    ExprStmt *exprstmt;
    FuncStmt *funcstmt;
    SelectStmt *selectstmt;
    IterStmt *iterstmt;
    RetStmt *retstmt;
    Statement *statement;
    SimpleExpr *simpleexpr;
    LogicExpr *logicexpr;
    LocalDecls *localdecls;
    Stmts *stmts;
    AddiExpr *addiexpr;
    RelOp *relop;
    AddOp *addop;
    MulOp *mulop;
    LogOp *logop;
    Term *term;
    Factor *factor;
    Call *call;
    Variable *variable;
    Args *args;

    DeclList *decllist;
    LocalList *locallist;
    ParamList *paramlist;
    StmtList *stmtlist;
    ExprList *exprlist;
    VarList *varlist;
    ArgList *arglist;
}

%token<iVal> INTEGER
%token<sVal> IDENTIFIER SYS_TYPE VOID STRING
%token<fVal> FLOAT
%token<cVal> CHAR

%type<program>                          program
%type<decllist>                         decl_list
%type<decl>                             decl
%type<vardecl>                          var_decl
%type<funcdecl>                         fun_decl
%type<typespec>                         type_specifier
%type<params>                           params
%type<param>                            param
%type<comstmt>                          compound_stmt
%type<paramlist>                        param_list
%type<locallist>                        local_decls
%type<stmtlist>                         stmt_list
%type<statement>                        stmt 
%type<exprstmt>                         expr_stmt
%type<funcstmt>                         function_stmt
%type<selectstmt>                       selection_stmt
%type<iterstmt>                         iteration_stmt
%type<retstmt>                          return_stmt
%type<exprstmt>                         expr 
%type<varlist>                          var_list
%type<logicexpr>                        logic_expr
%type<simpleexpr>                       simple_expr
%type<variable>                         var
%type<addiexpr>                         additive_expr
%type<term>                             term
%type<factor>                           factor
%type<args>                             args
%type<arglist>                          arg_list
%type<exprlist>                         expr_list
%type<relop>                            relop
%type<addop>                            addop
%type<mulop>                            mulop
%type<logop>                            logop
%type<call>                             call
~~~

定义规则中的非终结符：

~~~C
%token  LP RP LB RB LCP RCP
        DOT COMMA COLON UNDERSCORE
        MUL DIV PLUS MINUS MOD
        GE GT LE LT EQUAL UNEQUAL
        ASSIGN
        SEMI
        AND OR NOT
        IF ELSE WHILE FOR
        FUNC RETURN
~~~

为了防止出现dangling else问题，定义语法规则的优先级：

~~~C
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
~~~

之后，按照上述的语法规则，构建语法树。

##### 程序区

在程序区，首先解析输入参数，若没有发现待编译的文件，则打印提示信息。若参数正常，则解析文件内容，构建语法树，将语法树送入后端进行最终代码生成。

~~~C
int main(int argc, char** argv) {
    extern int yyparse(void);
	extern FILE* yyin;
    if (argc == 1) {
        printf("Please provide the file's name\n");
    } 
    else if (argc == 2) {
        if ((yyin = fopen(preProcess(std::string(argv[1])).c_str(), "r")) == NULL){
	        printf("Can't open file %s\n", argv[1]);
	        return 1;
	    }
        if (yyparse()) {
            fprintf(stderr, "Error\n");
            exit(1);
        }
    } 
    else {
        printf("Wrong parameters\n");
    }

    if(root != nullptr){
        std::ofstream os("ast.json");
        os << root->Visualize() << std::endl;
    }

    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    CodeGenerator generator;
    generator.generateCode(*root);
    ObjGen(generator);

    return 0;
}
~~~

