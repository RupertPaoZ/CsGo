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

std::string Program::Visualize()
{
    return Format<Decl>(this->declList);
}

std::string Decl::Visualize()
{
    return Format<Node>(this->children);
}

std::string TypeSpec::Visualize()
{
    return Format(this->childType);
}

std::string Params::Visualize()
{
    return Format(this->paramList);
}

std::string Param::Visualize()
{
    return Format(this->children);
}
std::string ComStmt::Visualize()
{
    return Format(this->children);
}
std::string LocalDecls::Visualize()
{
    return Format(this->localList);
}
std::string Stmts::Visualize()
{
    return Format(this->stmtList);
}
std::string Vars::Visualize()
{
    return Format(this->varList);
}
std::string Exprs::Visualize()
{
    return Format(this->exprList);
}
std::string ExprStmt::Visualize()
{
    if (isNull)
    {
        setName("empty");
        return Format();
    }
    else
        return Format(this->children);
}
std::string SelectStmt::Visualize()
{
    return Format(this->children);
}
std::string IterStmt::Visualize()
{
    return Format(this->children);
}
std::string RetStmt::Visualize()
{
    switch (this->retType)
    {
    case RET_INTEGER:
        return Format(this->integer);
    case RET_FLOAT:
        return Format(this->nfloat);
    default:
        return Format(this->exprStmt);
    }
}