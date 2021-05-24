%{
#include "ast.h"
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

%}

%union {
    int iVal;
    float fVal;
    std::string* sVal;
    Program *program;
}

%token  LP RP LB RB LCP RCP
        DOT COMMA COLON UNDERSCORE
        MUL DIV PLUS MINUS MOD
        GE GT LE LT EQUAL UNEQUAL
        ASSIGN
        SEMI
        AND OR NOT
        IF ELSE WHILE FOR
        FUNC RETURN

%token<iVal> INTEGER
%token<sVal> IDENTIFIER SYS_TYPE VOID
%token<fVal> FLOAT

%type<program>                         program
%type<sVal>                         decl_list
%type<sVal>                         decl
%type<sVal>                         var_decl
%type<sVal>                         fun_decl
%type<sVal>                         type_specifier
%type<sVal>                         params
%type<sVal>                         compound_stmt
%type<sVal>                         param_list
%type<sVal>                         local_decls
%type<sVal>                         stmt_list
%type<sVal>                         stmt 
%type<sVal>                         expr_stmt
%type<sVal>                         selection_stmt
%type<sVal>                         iteration_stmt
%type<sVal>                         return_stmt
%type<sVal>                         expr 
%type<sVal>                         var_list
%type<sVal>                         simple_expr
%type<sVal>                         var
%type<sVal>                         additive_expr
%type<sVal>                         term
%type<sVal>                         factor
%type<sVal>                         args
%type<sVal>                         arg_list

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%start program
%%

program: decl_list                      { 
                                            
                                            $$ = new Program();
                                            root = $$;
                                            
                                            printf("program -> decl_list\n");
                                        }
                                        ;
decl_list:
    decl_list decl                      {
                                            $$ = $1;
                                            /* $$->push_back(new Declaration()); */
                                            /* test */
                                            printf("decl_list -> decl_list decl\n");
                                        }
    | decl                              { $$ = $1; printf("decl_list -> decl\n"); }
                                        ;
decl: 
    var_decl                            { printf("decl -> var_decl\n"); }
    | fun_decl                          { printf("decl -> fun_decl\n"); }
                                        ;
var_decl:
    type_specifier IDENTIFIER SEMI      { printf("var_decl -> type_specifier identifier SEMI\n"); }
    | type_specifier IDENTIFIER LB INTEGER RB SEMI {printf("var_decl -> type_specifier identifier LB NUM RB\n"); }
                                        ;
type_specifier:
    SYS_TYPE                            { printf("type_specifier -> SYS_TYPE\n"); }
                                        ;
fun_decl:
    FUNC IDENTIFIER LP params RP LP params RP compound_stmt     
                                        { printf("fun_decl -> func identifier(params)(params) compound_stmt\n"); }
                                        ;
params:
    param_list                          { printf("params -> param_list\n" ); }
    | VOID                              { printf("params -> VOID\n");}
                                        ;
param_list:
    param_list COMMA param              { printf("param_list -> param_list COMMA param\n" ); }
    | param                             { printf("param_list -> param\n" ); }
                                        ;
param:
    IDENTIFIER type_specifier           { printf("param -> identifier type_specifier\n" ); }
    | IDENTIFIER LB RB type_specifier   { printf("param -> identifier [] type_specifier\n"); }
                                        ;
compound_stmt:
    LCP local_decls stmt_list RCP       { printf("compound_stmt -> {local_decls stmt_list}\n"); }
                                        ;
local_decls:
    local_decls var_decl                { printf("local_decls -> local_decls var_decl\n"); }
    | /* empty */                          
                                        ;
stmt_list:
    stmt_list stmt                      { printf("stmt_list -> stmt_list stmt\n"); }
    | /* empty */                       { printf("stmt_list -> empty\n"); }
                                        ;
stmt:
    expr_stmt                           { printf("stmt -> expr_stmt\n"); }
    | compound_stmt                     { printf("stmt -> compound_stmt\n"); }
    | selection_stmt                    { printf("stmt -> selection_stmt\n"); }
    | iteration_stmt                    { printf("stmt -> iteration-stmt\n"); }
    | return_stmt                       { printf("stmt -> return-stmt\n"); }
    | function_stmt                     { printf("stmt -> function-stmt\n"); }
                                        ;
expr_stmt:
    expr SEMI                           { printf("expr_stmt -> expr SEMI\n"); }
    | SEMI                              { printf("expr_stmt -> SEMI\n"); }
                                        ;
selection_stmt:
    IF LP simple_expr RP stmt  %prec LOWER_THAN_ELSE { printf("selection_stmt -> if (expr) stmt\n"); }
    | IF LP simple_expr RP stmt ELSE stmt      { printf("if (expr) stmt else stmt\n"); }
                                        ;
iteration_stmt:
    WHILE LP simple_expr RP stmt        { printf("while (expr) stmt\n"); }
                                        ;
return_stmt:
    RETURN INTEGER SEMI                 { printf("return_stmt -> return 0;\n"); }
    | RETURN FLOAT SEMI                 { printf("return_stmt -> return float;\n"); }
    | RETURN expr_stmt                  { printf("return expr_stmt;\n"); }
                                        ;
function_stmt:
    call SEMI                           { printf("function_stmt -> call\n"); }
                                        ;
expr:
    var_list ASSIGN expr_list           { printf("expr -> var_list ASSIGN expr_list\n"); }
                                        ;
expr_list:
    expr_list COMMA simple_expr         { printf("expr -> expr_list, simple_expr\n"); }
    | simple_expr                       { printf("expr_list -> simple_expr\n"); }
                                        ;
var_list:
    var_list COMMA var                  { printf("var_list -> var_list, var\n"); }
    | var                               { printf("var_list -> var\n"); }
    | UNDERSCORE                        { printf("var_list -> _\n"); }
                                        ;
var:
    IDENTIFIER                          { printf("var -> IDENTIFIER\n");}
    | IDENTIFIER LB simple_expr RB      { printf("var -> IDENTIFIER LB simple_expr RB\n"); }
                                        ;
simple_expr:
    additive_expr relop additive_expr   { printf("simple_expr -> additive_expr relop additive_expr\n"); }
    | additive_expr                     { printf("simple_expr -> additive_expr\n"); }
                                        ;
relop:
    LE                                  { printf("relop -> LE\n"); }
    | LT                                { printf("relop -> LT\n"); }
    | GT                                { printf("relop -> GT\n"); }
    | GE                                { printf("relop -> GE\n"); }
    | EQUAL                             { printf("relop -> EQUAL\n"); }
    | UNEQUAL                           { printf("relop -> UNEQUAL\n"); }
                                        ;
additive_expr:
    additive_expr addop term            { printf("additive_expr -> additive_expr addop term\n"); }
    | term                              { printf("additive_expr -> term\n"); }
                                        ;
addop:
    PLUS                                { printf("addop -> PLUS\n"); }
    | MINUS                             { printf("addop -> MINUS\n"); }
                                        ;
term:
    term mulop factor                   { printf("term -> term mulop factor\n"); }
    | factor                            { printf("term -> factor\n"); }
                                        ;
mulop:
    MUL                                 { printf("mulop -> MUL\n"); }
    | DIV                               { printf("mulop -> DIV\n"); }
    | MOD                               { printf("mulop -> MOD\n"); }
                                        ;
factor:
    LP simple_expr RP                   { printf("factor -> LP expr RP\n"); }
    | var                               { printf("factor -> var\n"); }
    | call                              { printf("factor -> call\n"); }
    | FLOAT                             { /* be careful */ printf("factor -> NUM\n"); }
    | INTEGER                           { printf("factor -> INTEGER\n"); }
                                        ;
call:
    IDENTIFIER LP args RP               { printf("call -> identifier LP args RP\n"); }
                                        ;
args:
    arg_list                            { printf("args -> arg_list\n"); }
    | /* empty */                       
                                        ;
arg_list:
    arg_list COMMA simple_expr          { printf("arg-list -> arg_list, expr\n"); }
    | simple_expr                       { printf("arg-list -> simple_expr\n"); }
                                        ;
%%

int main(int argc, char** argv) {
    extern int yyparse(void);
	extern FILE* yyin;
    if (argc == 1) {
        printf("Please provide the file's name\n");
    } 
    else if (argc == 2) {
        if ((yyin = fopen(argv[1], "r")) == NULL){
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

    return 0;
}