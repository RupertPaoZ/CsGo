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
class ExprStmt;
class Statement;
class SimpleExpr;
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
using DecList = std::vector<Decl *>;
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
    CG_CHAR
};

/*
 * the base class of the node in the tree
 */
class Node
{
public:
    Node() {}
    virtual ~Node() {}
};

class Program : public Node
{
public:
    Program() {}
    ~Program() {}
};

class Decl : public Node
{
public:
    Decl() {}
    ~Decl() {}
};
class VarDecl : public Decl
{
public:
    VarDecl() {}
    ~VarDecl() {}
};
class FuncDecl : public Decl
{
public:
    FuncDecl() {}
    ~FuncDecl() {}
};
class TypeSpec : public Node
{
public:
    TypeSpec() {}
    ~TypeSpec() {}
};
class Param : public Node
{
public:
    Param() {}
    ~Param() {}
};
class Params : public Node
{
public:
    Params() {}
    ~Params() {}
};
class ComStmt : public Statement
{
public:
    ComStmt() {}
    ~ComStmt() {}
};
class ExprStmt : public Statement
{
public:
    ExprStmt() {}
    ~ExprStmt() {}
};
class FuncStmt : public Statement
{
public:
    FuncStmt() {}
    ~FuncStmt() {}
};
class SelectStmt : public Statement
{
public:
    SelectStmt() {}
    ~SelectStmt() {}
};
class IterStmt : public IterStmt
{
public:
    IterStmt() {}
    ~IterStmt() {}
};
class RetStmt : public Statement
{
public:
    RetStmt() {}
    ~RetStmt() {}
};
class ExprStmt : public Node
{
public:
    ExprStmt() {}
    ~ExprStmt() {}
};
class Statement : public Node
{
public:
    Statement() {}
    ~Statement() {}
};
class SimpleExpr : public Node
{
public:
    SimpleExpr() {}
    ~SimpleExpr() {}
};
class AddiExpr : public SimpleExpr
{
public:
    AddiExpr() {}
    ~AddiExpr() {}
};
class RelOp : public Node
{
public:
    RelOp() {}
    ~RelOp() {}
};
class AddOp : public Node
{
public:
    AddOp() {}
    ~AddOp() {}
};
class MulOp : public Node
{
public:
    MulOp() {}
    ~MulOp() {}
};
class Term : public SimpleExpr
{
public:
    Term() {}
    ~Term() {}
};
class Factor : public Term
{
public:
    Factor() {}
    ~Factor() {}
};
class Call : public Factor
{
public:
    Call() {}
    ~Call() {}
};
class Variable : public Node
{
public:
    Variable() {}
    ~Variable() {}
};
class UnderScore : public Node
{
public:
    UnderScore() {}
    ~UnderScore() {}
};
class Args : public Node
{
public:
    Args() {}
    ~Args() {}
};
class Identifier : public SimpleExpr
{
public:
    Identifier() {}
    ~Identifier() {}
};
class SysType : public SimpleExpr
{
public:
    SysType() {}
    ~SysType() {}
};
class Integer : public SimpleExpr
{
public:
    Integer() {}
    ~Integer() {}
};
class Float : public SimpleExpr
{
public:
    Float() {}
    ~Float() {}
};
class Char : public SimpleExpr
{
public:
    Char() {}
    ~Char() {}
};

#endif
