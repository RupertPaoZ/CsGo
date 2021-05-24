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

class Program : public Node
{
private:
    DeclList *declList;

public:
    Program(DeclList *decllist) : Node("Program"), declList(decllist) {}
    std::string Visualize();
    ~Program() {}
};

class TypeSpec : public Node
{
private:
    SysType *childType;

public:
    TypeSpec(std::string childtype) : Node("TypeSpec")
    {
        childType = new SysType(childtype);
    }
    std::string Visualize();
    ~TypeSpec() { delete childType; }
};
class Param : public Node
{
private:
    Identifier *identifier;
    TypeSpec *typeSpec;
    bool isArray;
    NodeList *children;

public:
    Param(Identifier *iden, TypeSpec *typespec, bool isarray = false) : Node("Para"), identifier(iden), typeSpec(typespec), isArray(isarray)
    {
        this->children->push_back(dynamic_cast<Node *>(identifier));
        this->children->push_back(dynamic_cast<Node *>(typespec));
    }
    std::string Visualize();
    ~Param() {}
};
class Params : public Node
{
private:
    ParamList *paramList;

public:
    Params(ParamList *paramlist) : Node("Params"), paramList(paramlist) {}
    std::string Visualize();
    ~Params() {}
};
class Statement : public Node
{
public:
    Statement(std::string stmtname) : Node(stmtname) {}
    ~Statement() {}
};
class SimpleExpr : public Node
{
public:
    SimpleExpr(std::string simpleexprname) : Node(simpleexprname) {}
    ~SimpleExpr() {}
};
class LocalDecls : public Decl
{
private:
    LocalList *localList;

public:
    LocalDecls(LocalList *locallist) : Decl("LocalDecls"), localList(locallist) {}
    std::string Visualize();
    ~LocalDecls() {}
};
class Stmts : public Statement
{
private:
    StmtList *stmtList;

public:
    Stmts(StmtList *stmtlist) : Statement("LocalDecls"), stmtList(stmtlist) {}
    std::string Visualize();
    ~Stmts() {}
};
class ComStmt : public Statement
{
private:
    LocalDecls *localDecls;
    Stmts *stmts;
    NodeList *children;

public:
    ComStmt(LocalDecls *localdecls, Stmts *stmts) : Statement("ComStmt"), localDecls(localdecls), stmts(stmts)
    {
        this->children = new NodeList();
        this->children->push_back(dynamic_cast<Node *>(this->localDecls));
        this->children->push_back(dynamic_cast<Node *>(this->stmts));
    }
    std::string Visualize();
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
    NodeList *children;
    bool isNull;

public:
    ExprStmt(Vars *vars = nullptr, Exprs *exprs = nullptr, bool isnull = true) : Statement("ExprStmt"), vars(vars), exprs(exprs), isNull(isnull)
    {
        if (!isNull)
        {
            this->children = new NodeList();
            this->children->push_back(dynamic_cast<Node *>(vars));
            this->children->push_back(dynamic_cast<Node *>(exprs));
        }
    }
    std::string Visualize();
    ~ExprStmt() {}
};
class FuncStmt : public Statement
{
public:
    FuncStmt() : Statement("FuncStmt") {}
    std::string Visualize()
    {
        return Format();
    }
    ~FuncStmt() {}
};
class SelectStmt : public Statement
{
private:
    SimpleExpr *simpleExpr;
    Statement *ifStmt;
    Statement *elStmt;
    NodeList *children;
    bool isIfelse;

public:
    SelectStmt(SimpleExpr *simpleexpr, Statement *ifstmt, Statement *elstmt, bool isifelse = false) : Statement("SelectStmt"), simpleExpr(simpleexpr), ifStmt(ifstmt), elStmt(elstmt), isIfelse(isifelse)
    {
        children = new NodeList();
        this->children->push_back(dynamic_cast<Node *>(simpleExpr));
        this->children->push_back(dynamic_cast<Node *>(ifStmt));
        if (isIfelse)
            this->children->push_back(dynamic_cast<Node *>(elStmt));
    }
    std::string Visualize();
    ~SelectStmt() {}
};
class IterStmt : public Statement
{
private:
    SimpleExpr *simpleExpr;
    Statement *statement;
    NodeList *children;

public:
    IterStmt(SimpleExpr *simpleexpr, Statement *stmt) : Statement("IterStmt"), simpleExpr(simpleexpr), statement(stmt)
    {
        this->children = new NodeList();
        this->children->push_back(dynamic_cast<Node *>(this->simpleExpr));
        this->children->push_back(dynamic_cast<Node *>(this->statement));
    }
    std::string Visualize();
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
class AddiExpr : public SimpleExpr
{
public:
    AddiExpr() : SimpleExpr("AddiExpr") {}
    std::string Visualize()
    {
        return Format();
    }
    ~AddiExpr() {}
};
class RelOp : public Node
{
public:
    RelOp() : Node("RelOp") {}
    std::string Visualize()
    {
        return Format();
    }
    ~RelOp() {}
};
class AddOp : public Node
{
public:
    AddOp() : Node("AddOp") {}
    std::string Visualize()
    {
        return Format();
    }
    ~AddOp() {}
};
class MulOp : public Node
{
public:
    MulOp() : Node("MulOp") {}
    std::string Visualize()
    {
        return Format();
    }
    ~MulOp() {}
};
class Term : public SimpleExpr
{
public:
    Term(std::string termname) : SimpleExpr(termname) {}
    std::string Visualize()
    {
        return Format();
    }
    ~Term() {}
};
class Factor : public Term
{
public:
    Factor(std::string factorname) : Term(factorname) {}
    std::string Visualize()
    {
        return Format();
    }
    ~Factor() {}
};
class Call : public Factor
{
public:
    Call() : Factor("Call") {}
    std::string Visualize()
    {
        return Format();
    }
    ~Call() {}
};
class Variable : public Node
{
public:
    Variable() : Node("Variable") {}
    std::string Visualize()
    {
        return Format();
    }
    ~Variable() {}
};
class UnderScore : public Node
{
public:
    UnderScore() : Node("UnderScore") {}
    std::string Visualize()
    {
        return Format();
    }
    ~UnderScore() {}
};
class Args : public Node
{
public:
    Args() : Node("Args") {}
    std::string Visualize()
    {
        return Format();
    }
    ~Args() {}
};
class Identifier : public SimpleExpr
{
public:
    Identifier() : SimpleExpr("Identifier") {}
    std::string Visualize()
    {
        return Format();
    }
    ~Identifier() {}
};
class SysType : public Node
{
private:
    BuildInType dataType;

public:
    SysType(std::string datatype) : Node(datatype)
    {
        if (datatype == "int")
            dataType = CG_INTEGER;
        else if (datatype == "float")
            dataType = CG_FLOAT;
        else if (datatype == "char")
            dataType = CG_CHAR;
        else
            dataType = CG_VOID;
    }
    ~SysType() {}
};
class Void : public SysType
{
public:
    Void() : SysType("void") {}
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
public:
    Float() : SysType("Float") {}
    ~Float() {}
};
class Char : public SysType
{
public:
    Char() : SysType("Char") {}
    ~Char() {}
};
class Decl : public Node
{
protected:
    NodeList *children;

public:
    Decl(std::string declname) : Node(declname) {}
    std::string Visualize();
    ~Decl() { delete children; }
};
class VarDecl : public Decl
{
private:
    TypeSpec *typeSpec;
    Identifier *identifier;
    Integer *integer;
    bool isArray;

public:
    VarDecl(TypeSpec *typespec, Identifier *iden, Integer *inte = nullptr, bool isarray = false) : Decl("VarDecl"), typeSpec(typespec), identifier(iden), integer(inte), isArray(isarray)
    {
        this->children = new NodeList();
        this->children->push_back(dynamic_cast<Node *>(this->typeSpec));
        this->children->push_back(dynamic_cast<Node *>(this->identifier));
        if (this->isArray)
            this->children->push_back(dynamic_cast<Node *>(this->integer));
    }
    ~VarDecl() {}
};
class FuncDecl : public Decl
{
private:
    Identifier *identifier;
    Params *inParams, *outParams;
    ComStmt *comStmt;

public:
    FuncDecl(Identifier *iden, Params *inparams, Params *outparams, ComStmt *comstmt) : Decl("FuncDecl"), identifier(iden), inParams(inparams), outParams(outparams), comStmt(comstmt)
    {
        this->children = new NodeList();
        this->children->push_back(dynamic_cast<Node *>(this->identifier));
        this->children->push_back(dynamic_cast<Node *>(this->inParams));
        this->children->push_back(dynamic_cast<Node *>(this->outParams));
        this->children->push_back(dynamic_cast<Node *>(this->comStmt));
    }
    ~FuncDecl() {}
};

#endif
