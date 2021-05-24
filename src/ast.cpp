#include "ast.h"

std::string Node::Format()
{
    return "{ \"name\" : \"" + this->name + "\" }";
}
template <typename base>
std::string Node::Format(base *child)
{
    return "{ \"name\" : \"" + this->name + "\", \"children\" : [ " + child->Visualize() + " ] }";
}
template <typename base>
std::string Node::Format(std::vector<base *> *children)
{
    std::string subtree = "{ \"name\" : \"" + this->name + "\", \"children\" : [ ";
    for (auto child : *children)
    {
        subtree += child->Visualize() + ", ";
    }
    subtree[subtree.size() - 2] = ' ';
    return subtree + "] }";
}
void Node::setName(std::string newName)
{
    this->name = newName;
}

NodeWithChildren::NodeWithChildren(std::string name) : Node(name)
{
    this->children = new NodeList();
}
std::string NodeWithChildren::Visualize()
{
    return Format<Node>(this->children);
}

RelOp::RelOp(RelType reltype) : Node("RelOp"), relType(reltype)
{
    switch (reltype)
    {
    case REL_LE:
        this->setName("LE");
        break;
    case REL_LT:
        this->setName("LT");
        break;
    case REL_GE:
        this->setName("GE");
        break;
    case REL_GT:
        this->setName("GT");
        break;
    case REL_EQ:
        this->setName("EQUAL");
        break;
    case REL_UNE:
        this->setName("UNEQUAL");
        break;
    default:
        break;
    }
}

AddOp::AddOp(bool isplus) : Node("AddOp"), isPuls(isplus)
{
    if (this->isPuls)
        this->setName("Plus");
    else
        this->setName("Minus");
}

MulOp::MulOp(MulType multype) : Node("MulOp"), mulType(multype)
{
    switch (multype)
    {
    case MT_MUL:
        this->setName("Mul");
        break;
    case MT_DIV:
        this->setName("Div");
        break;
    case MT_MOD:
        this->setName("Mod");
        break;
    }
}

SysType::SysType(std::string datatype) : Node(datatype)
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

TypeSpec::TypeSpec(std::string childtype) : Node("TypeSpec")
{
    childType = new SysType(childtype);
}
std::string TypeSpec::Visualize()
{
    return Format<SysType>(this->childType);
}

std::string Program::Visualize()
{
    return Format<Decl>(this->declList);
}

Param::Param(Identifier *iden, TypeSpec *typespec, bool isarray) : NodeWithChildren("Para"), identifier(iden), typeSpec(typespec), isArray(isarray)
{
    this->children->push_back(dynamic_cast<Node *>(identifier));
    this->children->push_back(dynamic_cast<Node *>(typespec));
}

std::string Params::Visualize()
{
    return Format<Param>(this->paramList);
}

std::string LocalDecls::Visualize()
{
    return Format<VarDecl>(this->localList);
}

std::string Stmts::Visualize()
{
    return Format<Statement>(this->stmtList);
}

std::string Vars::Visualize()
{
    return Format<Variable>(this->varList);
}

std::string Exprs::Visualize()
{
    return Format<SimpleExpr>(this->exprList);
}

std::string ExprStmt::Visualize()
{
    if (isNull)
    {
        setName("empty");
        return Format();
    }
    else
        return Format<Node>(this->children);
}

std::string RetStmt::Visualize()
{
    switch (this->retType)
    {
    case RET_INTEGER:
        return Format<Integer>(this->integer);
    case RET_FLOAT:
        return Format<Float>(this->nfloat);
    default:
        return Format<ExprStmt>(this->exprStmt);
    }
}

std::string FuncStmt::Visualize()
{
    return Format<Call>(this->call);
}

std::string Factor::Visualize()
{
    switch (this->factorType)
    {
    case FT_SIMPLEEXPR:
        Format<SimpleExpr>(this->simpleExpr);
        break;
    case FT_VAR:
        Format<Variable>(this->variable);
        break;
    case FT_CALL:
        Format<Call>(this->call);
        break;
    case FT_FLOAT:
        Format<Float>(this->nfloat);
        break;
    case FT_INTEGER:
        Format<Integer>(this->integer);
        break;
    }
}

std::string Args::Visualize()
{
    return Format<SimpleExpr>(this->argList);
}

Term::Term(Term *term, MulOp *mulop, Factor *factor, bool isrec) : NodeWithChildren("Term"), term(term), mulOp(mulop), factor(factor), isRec(isrec)
{
    this->children->push_back(dynamic_cast<Node *>(this->term));
    if (this->isRec)
    {
        this->children->push_back(dynamic_cast<Node *>(this->mulOp));
        this->children->push_back(dynamic_cast<Node *>(this->factor));
    }
}

Call::Call(Identifier *iden, Args *args) : NodeWithChildren("Call"), identifier(iden), args(args)
{
    this->children->push_back(dynamic_cast<Node *>(this->identifier));
    this->children->push_back(dynamic_cast<Node *>(this->args));
}

Variable::Variable(Identifier *iden, SimpleExpr *simpleexpr, bool isarray) : NodeWithChildren("Variable"), identifier(iden), simpleExpr(simpleexpr), isArray(isarray)
{
    this->children->push_back(dynamic_cast<Node *>(identifier));
    this->children->push_back(dynamic_cast<Node *>(simpleExpr));
}

AddiExpr::AddiExpr(AddiExpr *addiexpr, AddOp *addop, Term *term, bool isrec) : NodeWithChildren("AddiExpr"), addiExpr(addiexpr), addOp(addop), term(term), isRec(isrec)
{
    this->children->push_back(dynamic_cast<Node *>(this->addiExpr));
    if (this->isRec)
    {
        this->children->push_back(dynamic_cast<Node *>(this->addOp));
        this->children->push_back(dynamic_cast<Node *>(this->term));
    }
}

SimpleExpr::SimpleExpr(AddiExpr *addiexpr1, RelOp *relop, AddiExpr *addiexpr2, bool iscompareexpr) : NodeWithChildren("SimpleExpr"), addiExpr1(addiexpr1), relOp(relop), addiExpr2(addiexpr2), isCompareExpr(iscompareexpr)
{
    this->children->push_back(dynamic_cast<Node *>(addiExpr1));
    if (this->isCompareExpr)
    {
        this->children->push_back(dynamic_cast<Node *>(relOp));
        this->children->push_back(dynamic_cast<Node *>(addiExpr2));
    }
}

ComStmt::ComStmt(LocalDecls *localdecls, Stmts *stmts) : Statement("ComStmt"), localDecls(localdecls), stmts(stmts)
{
    this->children->push_back(dynamic_cast<Node *>(this->localDecls));
    this->children->push_back(dynamic_cast<Node *>(this->stmts));
}

ExprStmt::ExprStmt(Vars *vars, Exprs *exprs, bool isnull) : Statement("ExprStmt"), vars(vars), exprs(exprs), isNull(isnull)
{
    if (!isNull)
    {
        this->children->push_back(dynamic_cast<Node *>(vars));
        this->children->push_back(dynamic_cast<Node *>(exprs));
    }
}

SelectStmt::SelectStmt(SimpleExpr *simpleexpr, Statement *ifstmt, Statement *elstmt, bool isifelse) : Statement("SelectStmt"), simpleExpr(simpleexpr), ifStmt(ifstmt), elStmt(elstmt), isIfelse(isifelse)
{
    this->children->push_back(dynamic_cast<Node *>(simpleExpr));
    this->children->push_back(dynamic_cast<Node *>(ifStmt));
    if (isIfelse)
        this->children->push_back(dynamic_cast<Node *>(elStmt));
}

IterStmt::IterStmt(SimpleExpr *simpleexpr, Statement *stmt) : Statement("IterStmt"), simpleExpr(simpleexpr), statement(stmt)
{
    this->children->push_back(dynamic_cast<Node *>(this->simpleExpr));
    this->children->push_back(dynamic_cast<Node *>(this->statement));
}

VarDecl::VarDecl(TypeSpec *typespec, Identifier *iden, Integer *inte, bool isarray) : Decl("VarDecl"), typeSpec(typespec), identifier(iden), integer(inte), isArray(isarray)
{
    this->children->push_back(dynamic_cast<Node *>(this->typeSpec));
    this->children->push_back(dynamic_cast<Node *>(this->identifier));
    if (this->isArray)
        this->children->push_back(dynamic_cast<Node *>(this->integer));
}

FuncDecl::FuncDecl(Identifier *iden, Params *inparams, Params *outparams, ComStmt *comstmt) : Decl("FuncDecl"), identifier(iden), inParams(inparams), outParams(outparams), comStmt(comstmt)
{
    this->children->push_back(dynamic_cast<Node *>(this->identifier));
    this->children->push_back(dynamic_cast<Node *>(this->inParams));
    this->children->push_back(dynamic_cast<Node *>(this->outParams));
    this->children->push_back(dynamic_cast<Node *>(this->comStmt));
}

Decl::Decl(std::string declname) : NodeWithChildren(declname) {}

LocalDecls::LocalDecls(LocalList *locallist) : Decl("LocalDecls"), localList(locallist) {}

Stmts::Stmts(StmtList *stmtlist) : Statement("LocalDecls"), stmtList(stmtlist) {}