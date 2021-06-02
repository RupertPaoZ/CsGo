#ifndef AST_H
#define AST_H

#include <iostream>
#include <vector>
#include <llvm/IR/Value.h>

class CodeGenerator;

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
    CG_VOID,
    CG_LONG
};

enum LogType
{
    LOG_AND
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
    virtual llvm::Value* Generate(CodeGenerator & codeGen) = 0;
    virtual ~Node() {}
};

class NodeWithChildren : public Node
{
protected:
    NodeList *children;

public:
    NodeWithChildren(std::string name);
    std::string Visualize();
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~NodeWithChildren() { delete children; }
};

class LogOp : public Node
{
private:
    LogType logType;

public:
    LogOp(LogType logtype);
    LogType getOpType(){
        return logType;
    }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~LogOp() {}
};

class RelOp : public Node
{
private:
    RelType relType;

public:
    RelOp(RelType reltype);
    RelType getOpType(){
        return relType;
    }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~RelOp() {}
};

class AddOp : public Node
{
private:
    bool isPuls;

public:
    AddOp(bool isplus = true);
    bool getOpType(){
        return isPuls;
    }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~AddOp() {}
};

class MulOp : public Node
{
private:
    MulType mulType;

public:
    MulOp(MulType multype);
    MulType getOpType(){
        return mulType;
    }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~MulOp() {}
};

class Program : public Node
{
private:
    DeclList *declList;

public:
    Program(DeclList *decllist) : Node("Program"), declList(decllist) {}
    DeclList* getDeclList(){
        return declList;
    }
    std::string Visualize();
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Program() {}
};

class SysType : public Node
{
private:
    BuildInType dataType;

public:
    SysType(std::string datatype);
    BuildInType getType(){
        return dataType;
    }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~SysType() {}
};

class TypeSpec : public Node
{
private:
    SysType *childType;

public:
    TypeSpec(std::string *childtype);
    BuildInType getType(){
        return childType->getType();
    }
    std::string Visualize();
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~TypeSpec() { delete childType; }
};

class Identifier : public Node
{
private:
    std::string ID;

public:
    Identifier(std::string *id) : Node("Identifier"), ID(*id) {}
    std::string getID(){
        return ID;
    }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
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
    BuildInType getType(){
        return typeSpec->getType();
    }
    std::string getName(){
        return identifier->getID();
    }
    bool getIsArray(){
        return isArray;
    }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Param() {}
};

class Params : public Node
{
private:
    ParamList *paramList;
    bool isVoid;

public:
    Params() : Node("Void"), isVoid(true) {}
    Params(ParamList *paramlist) : Node("Params"), paramList(paramlist), isVoid(false) {}
    // std::vector<BuildInType> getTypes(){
    //     std::vector<BuildInType> argTypes;
    //     for(auto &thisParam: *(this->paramList)){
    //         argTypes.push_back(thisParam->getType());
    //     }
    //     return argTypes;
    // }
    ParamList* getParamList(){
        return paramList;
    }
    bool getVoid(){
        return isVoid;
    }
    std::string Visualize();
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Params() {}
};
class Statement : public NodeWithChildren
{
public:
    Statement(std::string stmtname) : NodeWithChildren(stmtname) {}
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Statement() {}
};

class Call : public NodeWithChildren
{
private:
    Identifier *identifier;
    Args *args;

public:
    Call(Identifier *iden, Args *args);
    std::string getCallName(){
        return identifier->getID();
    }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Call() {}
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
    std::string getCallName(){
        if(factorType==FT_CALL){
            return call->getCallName();
        }
        else{
            return "";
        }
    }
    Variable* getVariable(bool& isString){
        if(factorType==FT_STRING) isString = true;
        else isString = false;
        if(factorType==FT_VAR) return variable;
        return nullptr;
    }
    bool isString(){
        return factorType==FT_STRING;
    }
    std::string Visualize();
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
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
    std::string getCallName(){
        return factor->getCallName();
    }
    Variable* getVariable(bool& isString){
        if(mulOp!=nullptr){
            isString = false;
            return nullptr;
        }
        return factor->getVariable(isString);
    }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Term() {}
};

class Variable : public NodeWithChildren
{
private:
    Identifier *identifier;
    SimpleExpr *simpleExpr;
    bool isArray;

public:
    Variable() : NodeWithChildren("UnderScore"), identifier(nullptr), simpleExpr(nullptr), isArray(false) {}
    Variable(Identifier *iden, SimpleExpr *simpleexpr = nullptr, bool isarray = false);
    std::string getID(){
        if(identifier==nullptr) return "";
        return identifier->getID();
    }
    bool isPointer(){
        if(simpleExpr==nullptr && isArray)return true;
        else return false;
    }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Variable() {}
};

class UnderScore : public Variable
{
public:
    UnderScore() {}
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
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
    std::string getCallName(){
        return term->getCallName();
    }
    Variable* getVariable(bool& isString){
        if(addOp!=nullptr){
            isString = false;
            return nullptr;
        }
        return term->getVariable(isString);
    }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
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
    std::string getCallName(){
        return addiExpr1->getCallName();
    }
    Variable* getVariable(bool& isString){
        if(relOp!=nullptr){
            isString = false;
            return nullptr;
        }
        return addiExpr1->getVariable(isString);
    }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~SimpleExpr() {}
};

class LogicExpr : public NodeWithChildren
{
private:
    LogicExpr *logicExpr;
    LogOp *logOp;
    SimpleExpr *simpleExpr;
    bool isLogicExpr;

public:
    LogicExpr(SimpleExpr *simpleexpr , LogOp *logop = nullptr,  LogicExpr *logicexpr  = nullptr, bool islogicexpr = false);
    // std::string getCallName(){
    //     return addiExpr1->getCallName();
    // }
    // Variable* getVariable(bool& isString){
    //     if(relOp!=nullptr){
    //         isString = false;
    //         return nullptr;
    //     }
    //     return addiExpr1->getVariable(isString);
    // }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~LogicExpr() {}
};

class Decl : public NodeWithChildren
{
public:
    Decl(std::string declname);
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Decl() {}
};

class LocalDecls : public Decl
{
private:
    LocalList *localList;

public:
    LocalDecls(LocalList *locallist);
    std::string Visualize();
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~LocalDecls() {}
};

class Stmts : public Statement
{
private:
    StmtList *stmtList;

public:
    Stmts(StmtList *stmtlist);
    std::string Visualize();
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Stmts() {}
};
class ComStmt : public Statement
{
private:
    LocalDecls *localDecls;
    Stmts *stmts;

public:
    ComStmt(LocalDecls *localdecls, Stmts *stmts);
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~ComStmt() {}
};
class Vars : public Node
{
private:
    VarList *varList;

public:
    Vars(VarList *varlist) : Node("Vars"), varList(varlist) {}
    VarList* getVarlist(){
        return varList;
    }
    std::string Visualize();
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Vars() {}
};

class Exprs : public Node
{
private:
    ExprList *exprList;

public:
    Exprs(ExprList *exprlist) : Node("Exprs"), exprList(exprlist) {}
    ExprList* getExprlist(){
        return exprList;
    }
    std::string Visualize();
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
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
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~ExprStmt() {}
};

class FuncStmt : public Statement
{
private:
    Call *call;

public:
    FuncStmt(Call *call) : Statement("FuncStmt"), call(call) {}
    std::string Visualize();
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~FuncStmt() {}
};

class SelectStmt : public Statement
{
private:
    LogicExpr *logicExpr;
    Statement *ifStmt;
    Statement *elStmt;
    bool isIfelse;

public:
    SelectStmt(LogicExpr *logicexpr, Statement *ifstmt, Statement *elstmt = nullptr, bool isifelse = false);
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~SelectStmt() {}
};
class IterStmt : public Statement
{
private:
    LogicExpr *logicExpr;
    Statement *statement;

public:
    IterStmt(LogicExpr *logicexpr, Statement *stmt);
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~IterStmt() {}
};
class Void : public Node
{
public:
    Void() : Node("void") {}
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Void() {}
};
class RetStmt : public Statement
{
private:
    enum
    {
        RET_VOID,
        RET_SIMEXPR
    } retType;
    Exprs *exprs;
    Void *nvoid;

public:
    RetStmt() : Statement("RetStmt"), exprs(nullptr)
    {
        retType = RET_VOID;
        nvoid = new Void();
    }
    RetStmt(ExprList *exprlist) : Statement("RetStmt"), nvoid(nullptr)
    {
        retType = RET_SIMEXPR;
        this->exprs = new Exprs(exprlist);
    }
    std::string Visualize();
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~RetStmt() {}
};

class Args : public Node
{
private:
    ArgList *argList;

public:
    Args(ArgList *arglist = nullptr) : Node("Args"), argList(arglist) {}
    ArgList* getArgList(){
        return argList;
    }
    std::string Visualize();
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Args() {}
};

class Integer : public SysType
{
private:
    int value;

public:
    Integer(int v) : SysType("integer"), value(v) {}
    int getValue(){
        return value;
    }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Integer() {}
};
class Float : public SysType
{
private:
    float value;

public:
    Float(float v) : SysType("float"), value(v) {}
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Float() {}
};
class Char : public SysType
{
private:
    char value;

public:
    Char(char v) : SysType("char"), value(v) {}
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~Char() {}
};

class String : public SysType
{
private:
    std::string value;

public:
    String(std::string *v) : SysType("string"), value(*v)
    {
        // value = value.substr(1, value.length() - 2);
        std::cout << value << std::endl;
    }
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
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
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
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
    virtual llvm::Value* Generate(CodeGenerator & codeGen) override;
    ~FuncDecl() {}
};

#endif
