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
    LocalDecls *localdecls;
    Stmts *stmts;
    AddiExpr *addiexpr;
    RelOp *relop;
    AddOp *addop;
    MulOp *mulop;
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
%type<call>                             call

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

%start program
%%

program: decl_list                      { 
                                            $$ = new Program($1);
                                            root = $$;  
                                            printf("program -> decl_list\n");
                                        }
                                        ;
decl_list:
    decl_list decl                      {
                                            $$ = $1;
                                            $$->push_back($2);
                                            printf("decl_list -> decl_list decl\n");
                                        }
    | decl                              {   $$ = new DeclList();
                                            $$->push_back($1);
                                            printf("decl_list -> decl\n"); }
                                        ;
decl: 
    var_decl                            {   $$ = $1;
                                            printf("decl -> var_decl\n"); }
    | fun_decl                          {   $$ = $1;
                                            printf("decl -> fun_decl\n"); }
                                        ;
var_decl:
    type_specifier IDENTIFIER SEMI      {   $$ = new VarDecl($1, new Identifier($2));
                                            printf("var_decl -> type_specifier identifier SEMI\n"); }
    | type_specifier IDENTIFIER LB INTEGER RB SEMI {
                                            $$ = new VarDecl($1, new Identifier($2), new Integer($4), true);
                                            printf("var_decl -> type_specifier identifier LB NUM RB\n"); }
                                        ;
type_specifier:
    SYS_TYPE                            {   $$ = new TypeSpec($1);
                                            printf("type_specifier -> SYS_TYPE\n"); }
                                        ;
fun_decl:
    FUNC IDENTIFIER LP params RP LP params RP compound_stmt     
                                        {   $$ = new FuncDecl(new Identifier($2), $4, $7, $9);
                                            printf("fun_decl -> func identifier(params)(params) compound_stmt\n"); }
                                        ;
params:
    param_list                          {   $$ = new Params($1);
                                            printf("params -> param_list\n" ); }
    | VOID                              {   $$ = new Params();
                                            printf("params -> VOID\n");}
                                        ;
param_list:
    param_list COMMA param              {   $$ = $1;
                                            $$->push_back($3);
                                            printf("param_list -> param_list COMMA param\n" ); }
    | param                             {   $$ = new ParamList();
                                            $$->push_back($1);
                                            printf("param_list -> param\n" ); }
                                        ;
param:
    IDENTIFIER type_specifier           {   $$ = new Param(new Identifier($1), $2);
                                            printf("param -> identifier type_specifier\n" ); }
    | IDENTIFIER LB RB type_specifier   {   $$ = new Param(new Identifier($1), $4, true);
                                            printf("param -> identifier [] type_specifier\n"); }
                                        ;
compound_stmt:
    LCP local_decls stmt_list RCP       {   $$ = new ComStmt(new LocalDecls($2), new Stmts($3));
                                            printf("compound_stmt -> {local_decls stmt_list}\n"); }
                                        ;
local_decls:
    local_decls var_decl                {   $$ = $1;
                                            $1->push_back($2);
                                            printf("local_decls -> local_decls var_decl\n"); }
    | /* empty */                       {   $$ = new LocalList(); }
                                        ;
stmt_list:
    stmt_list stmt                      {   $$ = $1;
                                            $1->push_back($2);
                                            printf("stmt_list -> stmt_list stmt\n"); }
    | /* empty */                       {   $$ = new StmtList();
                                            printf("stmt_list -> empty\n"); }
                                        ;
stmt:
    expr_stmt                           {   $$ = $1;
                                            printf("stmt -> expr_stmt\n"); }
    | compound_stmt                     {   $$ = $1;
                                            printf("stmt -> compound_stmt\n"); }
    | selection_stmt                    {   $$ = $1;
                                            printf("stmt -> selection_stmt\n"); }
    | iteration_stmt                    {   $$ = $1;
                                            printf("stmt -> iteration-stmt\n"); }
    | return_stmt                       {   $$ = $1;
                                            printf("stmt -> return-stmt\n"); }
    | function_stmt                     {   $$ = $1;
                                            printf("stmt -> function-stmt\n"); }
                                        ;
expr_stmt:
    expr SEMI                           {   $$ = $1;
                                            printf("expr_stmt -> expr SEMI\n"); }
    | SEMI                              {   $$ = new ExprStmt();
                                            printf("expr_stmt -> SEMI\n"); }
                                        ;
selection_stmt:
    IF LP simple_expr RP stmt  %prec LOWER_THAN_ELSE { 
                                            $$ = new SelectStmt($3, $5);
                                            printf("selection_stmt -> if (expr) stmt\n"); }
    | IF LP simple_expr RP stmt ELSE stmt      { 
                                            $$ = new SelectStmt($3, $5, $7, true);
                                            printf("if (expr) stmt else stmt\n"); }
                                        ;
iteration_stmt:
    WHILE LP simple_expr RP stmt        {   $$ = new IterStmt($3, $5);
                                            printf("while (expr) stmt\n"); }
                                        ;
return_stmt:
    RETURN SEMI                         {   $$ = new RetStmt();
                                            printf("return_stmt -> return ;\n"); }
    | RETURN expr_list SEMI             {   $$ = new RetStmt($2);
                                            printf("return expr_list;\n"); }
                                        ;
function_stmt:
    call SEMI                           {   $$ = new FuncStmt($1);
                                            printf("function_stmt -> call\n"); }
                                        ;
expr:
    var_list ASSIGN expr_list           {   $$ = new ExprStmt(new Vars($1), new Exprs($3), false);
                                            printf("expr -> var_list ASSIGN expr_list\n"); }
                                        ;
expr_list:
    expr_list COMMA simple_expr         {   $$ = $1;
                                            $$->push_back($3);
                                            printf("expr -> expr_list, simple_expr\n"); }
    | simple_expr                       {   $$ = new ExprList();
                                            $$->push_back($1);
                                            printf("expr_list -> simple_expr\n"); }
                                        ;
var_list:
    var_list COMMA var                  {   $$ = $1;
                                            $$->push_back($3);
                                            printf("var_list -> var_list, var\n"); }
    | var                               {   $$ = new VarList();
                                            $$->push_back($1);
                                            printf("var_list -> var\n"); }
    | UNDERSCORE                        {   $$ = new VarList();
                                            $$->push_back(new UnderScore());
                                            printf("var_list -> _\n"); }
                                        ;
var:
    IDENTIFIER                          {   $$ = new Variable(new Identifier($1));
                                            printf("var -> IDENTIFIER\n");}
    | IDENTIFIER LB simple_expr RB      {   $$ = new Variable(new Identifier($1), $3, true);
                                            printf("var -> IDENTIFIER LB simple_expr RB\n"); }
                                        ;
simple_expr:
    additive_expr relop additive_expr   {   $$ = new SimpleExpr($1, $2, $3, true);
                                            printf("simple_expr -> additive_expr relop additive_expr\n"); }
    | additive_expr                     {   $$ = new SimpleExpr($1);
                                            printf("simple_expr -> additive_expr\n"); }
                                        ;
relop:
    LE                                  {   $$ = new RelOp(REL_LE);
                                            printf("relop -> LE\n"); }
    | LT                                {   $$ = new RelOp(REL_LT);
                                            printf("relop -> LT\n"); }
    | GT                                {   $$ = new RelOp(REL_GT);
                                            printf("relop -> GT\n"); }
    | GE                                {   $$ = new RelOp(REL_GE);
                                            printf("relop -> GE\n"); }
    | EQUAL                             {   $$ = new RelOp(REL_EQ);
                                            printf("relop -> EQUAL\n"); }
    | UNEQUAL                           {   $$ = new RelOp(REL_UNE);
                                            printf("relop -> UNEQUAL\n"); }
                                        ;
additive_expr:
    additive_expr addop term            {   $$ = new AddiExpr($3, $2, $1, true);
                                            printf("additive_expr -> additive_expr addop term\n"); }
    | term                              {   $$ = new AddiExpr($1);
                                            printf("additive_expr -> term\n"); }
                                        ;
addop:
    PLUS                                {   $$ = new AddOp();
                                            printf("addop -> PLUS\n"); }
    | MINUS                             {   $$ = new AddOp(false);
                                            printf("addop -> MINUS\n"); }
                                        ;
term:
    term mulop factor                   {   $$ = new Term($3, $2, $1, true);
                                            printf("term -> term mulop factor\n"); }
    | factor                            {   $$ = new Term($1);
                                            printf("term -> factor\n"); }
                                        ;
mulop:
    MUL                                 {   $$ = new MulOp(MT_MUL);
                                            printf("mulop -> MUL\n"); }
    | DIV                               {   $$ = new MulOp(MT_DIV);
                                            printf("mulop -> DIV\n"); }
    | MOD                               {   $$ = new MulOp(MT_MOD);
                                            printf("mulop -> MOD\n"); }
                                        ;
factor:
    LP simple_expr RP                   {   $$ = new Factor($2);
                                            printf("factor -> LP expr RP\n"); }
    | var                               {   $$ = new Factor($1);
                                            printf("factor -> var\n"); }
    | call                              {   $$ = new Factor($1);
                                            printf("factor -> call\n"); }
    | FLOAT                             {   $$ = new Factor(new Float($1));
                                            printf("factor -> FLOAT\n"); }
    | INTEGER                           {   $$ = new Factor(new Integer($1));
                                            printf("factor -> INTEGER\n"); }
    | CHAR                              {   $$ = new Factor(new Char($1));
                                            // printf("%c\n", $1);
                                            printf("factor -> CHAR\n"); }
    | STRING                            {   $$ = new Factor(new String($1));
                                            // printf("%s\n", (*$1).c_str());
                                            printf("factor -> STRING\n"); }
                                        ;
call:
    IDENTIFIER LP args RP               {   $$ = new Call(new Identifier($1), $3);
                                            printf("call -> identifier LP args RP\n"); }
                                        ;
args:
    arg_list                            {   $$ = new Args($1);
                                            printf("args -> arg_list\n"); }
    | /* empty */                       {   $$ = new Args(); }
                                        ;
arg_list:
    arg_list COMMA simple_expr          {   $$ = $1;
                                            $$->push_back($3);
                                            printf("arg-list -> arg_list, expr\n"); }
    | simple_expr                       {   $$ = new ArgList();
                                            $$->push_back($1);
                                            printf("arg-list -> simple_expr\n"); }
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