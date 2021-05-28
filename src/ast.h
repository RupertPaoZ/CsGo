<<<<<<< HEAD
#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>

/*Node of the tree*/
class Node;

/*terminal*/
class Identifier;
class SysType;
class Integer;
class Float;
class Char;
class String;
class UnderScore;
class Void;

/*nonterminal*/
class Program;
class Decl;
class VarDecl;
class FuncDecl;
class TypeSpec;
class Param;
class Params;
class ComStmt;
class ExprStmt;
class FuncStmt;
class SelectStmt;
class IterStmt;
class RetStmt;
class Statement;
class SimpleExpr;
class LocalDecls;
class Stmts;
class AddiExpr;
class RelOp;
class AddOp;
class MulOp;
class Term;
class Factor;
class Call;
class Variable;
class Args;

/*List*/
using NodeList = std::vector<Node *>;
using DeclList = std::vector<Decl *>;
using LocalList = std::vector<VarDecl *>;
using ParamList = std::vector<Param *>;
using StmtList = std::vector<Statement *>;
using ExprList = std::vector<SimpleExpr *>;
using VarList = std::vector<Variable *>;
using ArgList = std::vector<SimpleExpr *>;

/*
 * the buildin type of cs go
 */
enum BuildInType
{
    CG_INTEGER,
    CG_FLOAT,
    CG_CHAR,
    CG_STRING,
    CG_VOID
};

enum RelType
{
    REL_LE,
    REL_LT,
    REL_GT,
    REL_GE,
    REL_EQ,
    REL_UNE
};

enum MulType
{
    MT_MUL,
    MT_DIV,
    MT_MOD
};
/*
 * the base class of the node in the tree
 */
class Node
{
private:
    std::string name;

protected:
    /*
    * generate the json code of AST
    */
    std::string Format();
    template <typename base>
    std::string Format(base *child);
    template <typename base>
    std::string Format(std::vector<base *> *children);
    void setName(std::string newName);

public:
    Node(std::string nodename) : name(nodename) {}
    virtual std::string Visualize() { return Format(); }
    virtual ~Node() {}
};

class NodeWithChildren : public Node
{
protected:
    NodeList *children;

public:
    NodeWithChildren(std::string name);
    std::string Visualize();
    ~NodeWithChildren() { delete children; }
};

class RelOp : public Node
{
private:
    RelType relType;

public:
    RelOp(RelType reltype);
    ~RelOp() {}
};

class AddOp : public Node
{
private:
    bool isPuls;

public:
    AddOp(bool isplus = true);
    ~AddOp() {}
};

class MulOp : public Node
{
private:
    MulType mulType;

public:
    MulOp(MulType multype);
    ~MulOp() {}
};

class Program : public Node
{
private:
    DeclList *declList;

public:
    Program(DeclList *decllist) : Node("Program"), declList(decllist) {}
    std::string Visualize();
    ~Program() {}
};

class SysType : public Node
{
private:
    BuildInType dataType;

public:
    SysType(std::string datatype);
    ~SysType() {}
};

class TypeSpec : public Node
{
private:
    SysType *childType;

public:
    TypeSpec(std::string *childtype);
    std::string Visualize();
    ~TypeSpec() { delete childType; }
};

class Identifier : public Node
{
private:
    std::string ID;
public:
    Identifier(std::string *id) : Node("Identifier"), ID(*id) {}
    ~Identifier() {}
};
class Param : public NodeWithChildren
{
private:
    Identifier *identifier;
    TypeSpec *typeSpec;
    bool isArray;

public:
    Param(Identifier *iden, TypeSpec *typespec, bool isarray = false);
    ~Param() {}
};

class Params : public Node
{
private:
    ParamList *paramList;
    bool isVoid;

public:
    Params() : Node("Void"), isVoid(true){}
    Params(ParamList *paramlist) : Node("Params"), paramList(paramlist), isVoid(false) {}
    std::string Visualize();
    ~Params() {}
};
class Statement : public NodeWithChildren
{
public:
    Statement(std::string stmtname) : NodeWithChildren(stmtname) {}
    ~Statement() {}
};

class Factor : public Node
{
private:
    enum
    {
        FT_SIMPLEEXPR,
        FT_VAR,
        FT_CALL,
        FT_FLOAT,
        FT_INTEGER,
        FT_CHAR,
        FT_STRING
    } factorType;
    SimpleExpr *simpleExpr;
    Variable *variable;
    Call *call;
    Float *nfloat;
    Integer *integer;
    Char *nchar;
    String *nstring;

public:
    Factor(SimpleExpr *simpleexpr) : Node("factor"), simpleExpr(simpleexpr) { factorType = FT_SIMPLEEXPR; }
    Factor(Variable *variable) : Node("factor"), variable(variable) { factorType = FT_VAR; }
    Factor(Call *call) : Node("factor"), call(call) { factorType = FT_CALL; }
    Factor(Float *nfloat) : Node("factor"), nfloat(nfloat) { factorType = FT_FLOAT; }
    Factor(Integer *inte) : Node("factor"), integer(inte) { factorType = FT_INTEGER; }
    Factor(Char *nchar) : Node("factor"), nchar(nchar) { factorType = FT_CHAR; }
    Factor(String *nstring) : Node("factor"), nstring(nstring) { factorType = FT_STRING; }
    std::string Visualize();
    ~Factor() {}
};

class Term : public NodeWithChildren
{
private:
    Term *term;
    MulOp *mulOp;
    Factor *factor;
    bool isRec;

public:
    Term(Factor *factor, MulOp *mulop = nullptr, Term *term = nullptr, bool isrec = false);
    ~Term() {}
};

class Call : public NodeWithChildren
{
private:
    Identifier *identifier;
    Args *args;

public:
    Call(Identifier *iden, Args *args);
    ~Call() {}
};

class Variable : public NodeWithChildren
{
private:
    Identifier *identifier;
    SimpleExpr *simpleExpr;
    bool isArray;

public:
    Variable():NodeWithChildren("UnderScore"),identifier(nullptr),simpleExpr(nullptr),isArray(false){}
    Variable(Identifier *iden, SimpleExpr *simpleexpr = nullptr, bool isarray = false);
    ~Variable() {}
};

class UnderScore : public Variable
{
public:
    UnderScore() {}
    ~UnderScore() {}
};

class AddiExpr : public NodeWithChildren
{
private:
    AddiExpr *addiExpr;
    AddOp *addOp;
    Term *term;
    bool isRec;

public:
    AddiExpr(Term *term, AddOp *addop = nullptr, AddiExpr *addiexpr = nullptr, bool isrec = false);
    ~AddiExpr() {}
};

class SimpleExpr : public NodeWithChildren
{
private:
    AddiExpr *addiExpr1;
    RelOp *relOp;
    AddiExpr *addiExpr2;
    bool isCompareExpr;

public:
    SimpleExpr(AddiExpr *addiexpr1, RelOp *relop = nullptr, AddiExpr *addiexpr2 = nullptr, bool iscompareexpr = false);
    ~SimpleExpr() {}
};

class Decl : public NodeWithChildren
{
public:
    Decl(std::string declname);
    ~Decl() {}
};

class LocalDecls : public Decl
{
private:
    LocalList *localList;

public:
    LocalDecls(LocalList *locallist);
    std::string Visualize();
    ~LocalDecls() {}
};

class Stmts : public Statement
{
private:
    StmtList *stmtList;

public:
    Stmts(StmtList *stmtlist);
    std::string Visualize();
    ~Stmts() {}
};
class ComStmt : public Statement
{
private:
    LocalDecls *localDecls;
    Stmts *stmts;

public:
    ComStmt(LocalDecls *localdecls, Stmts *stmts);
    ~ComStmt() {}
};
class Vars : public Node
{
private:
    VarList *varList;

public:
    Vars(VarList *varlist) : Node("Vars"), varList(varlist) {}
    std::string Visualize();
    ~Vars() {}
};

class Exprs : public Node
{
private:
    ExprList *exprList;

public:
    Exprs(ExprList *exprlist) : Node("Exprs"), exprList(exprlist) {}
    std::string Visualize();
    ~Exprs() {}
};

class ExprStmt : public Statement
{
private:
    Vars *vars;
    Exprs *exprs;
    bool isNull;

public:
    ExprStmt(Vars *vars = nullptr, Exprs *exprs = nullptr, bool isnull = true);
    std::string Visualize();
    ~ExprStmt() {}
};

class FuncStmt : public Statement
{
private:
    Call *call;

public:
    FuncStmt(Call *call) : Statement("FuncStmt"), call(call) {}
    std::string Visualize();
    ~FuncStmt() {}
};

class SelectStmt : public Statement
{
private:
    SimpleExpr *simpleExpr;
    Statement *ifStmt;
    Statement *elStmt;
    bool isIfelse;

public:
    SelectStmt(SimpleExpr *simpleexpr, Statement *ifstmt, Statement *elstmt = nullptr, bool isifelse = false);
    ~SelectStmt() {}
};
class IterStmt : public Statement
{
private:
    SimpleExpr *simpleExpr;
    Statement *statement;

public:
    IterStmt(SimpleExpr *simpleexpr, Statement *stmt);
    ~IterStmt() {}
};
class RetStmt : public Statement
{
private:
    enum
    {
        RET_INTEGER,
        RET_FLOAT,
        RET_CHAR,
        RET_STRING,
        RET_EXPSTMT
    } retType;
    Integer *integer;
    Float *nfloat;
    Char *nchar;
    String *nstring;
    ExprStmt *exprStmt;

public:
    RetStmt(Integer *inte) : Statement("RetStmt"), integer(inte)
    {
        retType = RET_INTEGER;
    }
    RetStmt(Float *nfloat) : Statement("RetStmt"), nfloat(nfloat)
    {
        retType = RET_FLOAT;
    }
    RetStmt(Char *nchar) : Statement("RetStmt"), nchar(nchar)
    {
        retType = RET_CHAR;
    }
    RetStmt(String *nstring) : Statement("RetStmt"), nstring(nstring)
    {
        retType = RET_STRING;
    }
    RetStmt(ExprStmt *exprstmt) : Statement("RetStmt"), exprStmt(exprstmt)
    {
        retType = RET_EXPSTMT;
    }
    std::string Visualize();
    ~RetStmt() {}
};

class Args : public Node
{
private:
    ArgList *argList;

public:
    Args(ArgList *arglist=nullptr) : Node("Args"), argList(arglist) {}
    std::string Visualize();
    ~Args() {}
};
class Void : public Node
{
public:
    Void() : Node("void") {}
    ~Void() {}
};
class Integer : public SysType
{
private:
    int value;

public:
    Integer(int v) : SysType("integer"), value(v) {}
    ~Integer() {}
};
class Float : public SysType
{
private:
    float value;

public:
    Float(float v) : SysType("float"), value(v) {}
    ~Float() {}
};
class Char : public SysType
{
private:
    char value;

public:
    Char(char v) : SysType("char"), value(v) {}
    ~Char() {}
};

class String : public SysType
{
private:
    std::string value;
public:
    String(std::string* v) : SysType("string"), value(*v){
        value = value.substr(1, value.length()-2);
        std::cout << value << std::endl;
    }
    ~String() {}
};

class VarDecl : public Decl
{
private:
    TypeSpec *typeSpec;
    Identifier *identifier;
    Integer *integer;
    bool isArray;

public:
    VarDecl(TypeSpec *typespec, Identifier *iden, Integer *inte = nullptr, bool isarray = false);
    ~VarDecl() {}
};

class FuncDecl : public Decl
{
private:
    Identifier *identifier;
    Params *inParams, *outParams;
    ComStmt *comStmt;

public:
    FuncDecl(Identifier *iden, Params *inparams, Params *outparams, ComStmt *comstmt);
    ~FuncDecl() {}
};

#endif
=======
#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>

/*Node of the tree*/
class Node;

/*terminal*/
class Identifier;
class SysType;
class Integer;
class Float;
class Char;
class UnderScore;
class Void;

/*nonterminal*/
class Program;
class Decl;
class VarDecl;
class FuncDecl;
class TypeSpec;
class Param;
class Params;
class ComStmt;
class ExprStmt;
class FuncStmt;
class SelectStmt;
class IterStmt;
class RetStmt;
class Statement;
class SimpleExpr;
class LocalDecls;
class Stmts;
class AddiExpr;
class RelOp;
class AddOp;
class MulOp;
class Term;
class Factor;
class Call;
class Variable;
class Args;

/*List*/
using NodeList = std::vector<Node *>;
using DeclList = std::vector<Decl *>;
using LocalList = std::vector<VarDecl *>;
using ParamList = std::vector<Param *>;
using StmtList = std::vector<Statement *>;
using ExprList = std::vector<SimpleExpr *>;
using VarList = std::vector<Variable *>;
using ArgList = std::vector<SimpleExpr *>;

/*
 * the buildin type of cs go
 */
enum BuildInType
{
    CG_INTEGER,
    CG_FLOAT,
    CG_CHAR,
    CG_VOID
};

enum RelType
{
    REL_LE,
    REL_LT,
    REL_GT,
    REL_GE,
    REL_EQ,
    REL_UNE
};

enum MulType
{
    MT_MUL,
    MT_DIV,
    MT_MOD
};
/*
 * the base class of the node in the tree
 */
class Node
{
private:
    std::string name;

protected:
    /*
    * generate the json code of AST
    */
    std::string Format();
    template <typename base>
    std::string Format(base *child);
    template <typename base>
    std::string Format(std::vector<base *> *children);
    void setName(std::string newName);

public:
    Node(std::string nodename) : name(nodename) {}
    virtual std::string Visualize() { return Format(); }
    virtual ~Node() {}
};

class NodeWithChildren : public Node
{
protected:
    NodeList *children;

public:
    NodeWithChildren(std::string name);
    std::string Visualize();
    ~NodeWithChildren() { delete children; }
};

class RelOp : public Node
{
private:
    RelType relType;

public:
    RelOp(RelType reltype);
    ~RelOp() {}
};

class AddOp : public Node
{
private:
    bool isPuls;

public:
    AddOp(bool isplus = true);
    ~AddOp() {}
};

class MulOp : public Node
{
private:
    MulType mulType;

public:
    MulOp(MulType multype);
    ~MulOp() {}
};

class Program : public Node
{
private:
    DeclList *declList;

public:
    Program(DeclList *decllist) : Node("Program"), declList(decllist) {}
    std::string Visualize();
    ~Program() {}
};

class SysType : public Node
{
private:
    BuildInType dataType;

public:
    SysType(std::string datatype);
    ~SysType() {}
};

class TypeSpec : public Node
{
private:
    SysType *childType;

public:
    TypeSpec(std::string *childtype);
    std::string Visualize();
    ~TypeSpec() { delete childType; }
};

class Identifier : public Node
{
private:
    std::string ID;
public:
    Identifier(std::string *id) : Node("Identifier"), ID(*id) {}
    ~Identifier() {}
};
class Param : public NodeWithChildren
{
private:
    Identifier *identifier;
    TypeSpec *typeSpec;
    bool isArray;

public:
    Param(Identifier *iden, TypeSpec *typespec, bool isarray = false);
    ~Param() {}
};

class Params : public Node
{
private:
    ParamList *paramList;
    bool isVoid;

public:
    Params() : Node("Void"), isVoid(true){}
    Params(ParamList *paramlist) : Node("Params"), paramList(paramlist), isVoid(false) {}
    std::string Visualize();
    ~Params() {}
};
class Statement : public NodeWithChildren
{
public:
    Statement(std::string stmtname) : NodeWithChildren(stmtname) {}
    ~Statement() {}
};

class Factor : public Node
{
private:
    enum
    {
        FT_SIMPLEEXPR,
        FT_VAR,
        FT_CALL,
        FT_FLOAT,
        FT_INTEGER
    } factorType;
    SimpleExpr *simpleExpr;
    Variable *variable;
    Call *call;
    Float *nfloat;
    Integer *integer;

public:
    Factor(SimpleExpr *simpleexpr) : Node("factor"), simpleExpr(simpleexpr) { factorType = FT_SIMPLEEXPR; }
    Factor(Variable *variable) : Node("factor"), variable(variable) { factorType = FT_VAR; }
    Factor(Call *call) : Node("factor"), call(call) { factorType = FT_CALL; }
    Factor(Float *nfloat) : Node("factor"), nfloat(nfloat) { factorType = FT_FLOAT; }
    Factor(Integer *inte) : Node("factor"), integer(inte) { factorType = FT_INTEGER; }
    std::string Visualize();
    ~Factor() {}
};

class Term : public NodeWithChildren
{
private:
    Term *term;
    MulOp *mulOp;
    Factor *factor;
    bool isRec;

public:
    Term(Factor *factor, MulOp *mulop = nullptr, Term *term = nullptr, bool isrec = false);
    ~Term() {}
};

class Call : public NodeWithChildren
{
private:
    Identifier *identifier;
    Args *args;

public:
    Call(Identifier *iden, Args *args);
    ~Call() {}
};

class Variable : public NodeWithChildren
{
private:
    Identifier *identifier;
    SimpleExpr *simpleExpr;
    bool isArray;

public:
    Variable():NodeWithChildren("UnderScore"),identifier(nullptr),simpleExpr(nullptr),isArray(false){}
    Variable(Identifier *iden, SimpleExpr *simpleexpr = nullptr, bool isarray = false);
    ~Variable() {}
};

class UnderScore : public Variable
{
public:
    UnderScore() {}
    ~UnderScore() {}
};

class AddiExpr : public NodeWithChildren
{
private:
    AddiExpr *addiExpr;
    AddOp *addOp;
    Term *term;
    bool isRec;

public:
    AddiExpr(Term *term, AddOp *addop = nullptr, AddiExpr *addiexpr = nullptr, bool isrec = false);
    ~AddiExpr() {}
};

class SimpleExpr : public NodeWithChildren
{
private:
    AddiExpr *addiExpr1;
    RelOp *relOp;
    AddiExpr *addiExpr2;
    bool isCompareExpr;

public:
    SimpleExpr(AddiExpr *addiexpr1, RelOp *relop = nullptr, AddiExpr *addiexpr2 = nullptr, bool iscompareexpr = false);
    ~SimpleExpr() {}
};

class Decl : public NodeWithChildren
{
public:
    Decl(std::string declname);
    ~Decl() {}
};

class LocalDecls : public Decl
{
private:
    LocalList *localList;

public:
    LocalDecls(LocalList *locallist);
    std::string Visualize();
    ~LocalDecls() {}
};

class Stmts : public Statement
{
private:
    StmtList *stmtList;

public:
    Stmts(StmtList *stmtlist);
    std::string Visualize();
    ~Stmts() {}
};
class ComStmt : public Statement
{
private:
    LocalDecls *localDecls;
    Stmts *stmts;

public:
    ComStmt(LocalDecls *localdecls, Stmts *stmts);
    ~ComStmt() {}
};
class Vars : public Node
{
private:
    VarList *varList;

public:
    Vars(VarList *varlist) : Node("Vars"), varList(varlist) {}
    std::string Visualize();
    ~Vars() {}
};

class Exprs : public Node
{
private:
    ExprList *exprList;

public:
    Exprs(ExprList *exprlist) : Node("Exprs"), exprList(exprlist) {}
    std::string Visualize();
    ~Exprs() {}
};

class ExprStmt : public Statement
{
private:
    Vars *vars;
    Exprs *exprs;
    bool isNull;

public:
    ExprStmt(Vars *vars = nullptr, Exprs *exprs = nullptr, bool isnull = true);
    std::string Visualize();
    ~ExprStmt() {}
};

class FuncStmt : public Statement
{
private:
    Call *call;

public:
    FuncStmt(Call *call) : Statement("FuncStmt"), call(call) {}
    std::string Visualize();
    ~FuncStmt() {}
};

class SelectStmt : public Statement
{
private:
    SimpleExpr *simpleExpr;
    Statement *ifStmt;
    Statement *elStmt;
    bool isIfelse;

public:
    SelectStmt(SimpleExpr *simpleexpr, Statement *ifstmt, Statement *elstmt = nullptr, bool isifelse = false);
    ~SelectStmt() {}
};
class IterStmt : public Statement
{
private:
    SimpleExpr *simpleExpr;
    Statement *statement;

public:
    IterStmt(SimpleExpr *simpleexpr, Statement *stmt);
    ~IterStmt() {}
};
class RetStmt : public Statement
{
private:
    enum
    {
        RET_INTEGER,
        RET_FLOAT,
        RET_EXPSTMT
    } retType;
    Integer *integer;
    Float *nfloat;
    ExprStmt *exprStmt;

public:
    RetStmt(Integer *inte) : Statement("RetStmt"), integer(inte)
    {
        retType = RET_INTEGER;
    }
    RetStmt(Float *nfloat) : Statement("RetStmt"), nfloat(nfloat)
    {
        retType = RET_FLOAT;
    }
    RetStmt(ExprStmt *exprstmt) : Statement("RetStmt"), exprStmt(exprstmt)
    {
        retType = RET_EXPSTMT;
    }
    std::string Visualize();
    ~RetStmt() {}
};

class Args : public Node
{
private:
    ArgList *argList;

public:
    Args(ArgList *arglist=nullptr) : Node("Args"), argList(arglist) {}
    std::string Visualize();
    ~Args() {}
};
class Void : public Node
{
public:
    Void() : Node("void") {}
    ~Void() {}
};
class Integer : public SysType
{
private:
    int value;

public:
    Integer(int v) : SysType("Integer"), value(v) {}
    ~Integer() {}
};
class Float : public SysType
{
private:
    float value;

public:
    Float(float v) : SysType("Float"), value(v) {}
    ~Float() {}
};
class Char : public SysType
{
private:
    char value;

public:
    Char(char v) : SysType("Char"), value(v) {}
    ~Char() {}
};

class VarDecl : public Decl
{
private:
    TypeSpec *typeSpec;
    Identifier *identifier;
    Integer *integer;
    bool isArray;

public:
    VarDecl(TypeSpec *typespec, Identifier *iden, Integer *inte = nullptr, bool isarray = false);
    ~VarDecl() {}
};

class FuncDecl : public Decl
{
private:
    Identifier *identifier;
    Params *inParams, *outParams;
    ComStmt *comStmt;

public:
    FuncDecl(Identifier *iden, Params *inparams, Params *outparams, ComStmt *comstmt);
    ~FuncDecl() {}
};

#endif
>>>>>>> main
