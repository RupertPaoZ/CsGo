### AST类继承体系

`NodeWithChildren`就是带有不同类型孩子的`Node`

#### Nonterminal
| Class | Base class |
| --- | --- |
| Program | Node |
| Decl | NodeWithChildren |
| VarDecl | Decl |
| FuncDecl | Decl |
| TypeSpec | Node |
| Param | NodeWithChildren |
| Params | Node |
| Statement | NodeWithChildren |
| ComStmt | Statement |
| ExprStmt | Statement |
| FuncStmt | Statement |
| SelectStmt | Statement |
| IterStmt | Statement|
| RetStmt | Statement |
| Stmts | Statement |
| SimpleExpr | NodeWithChildren |
| LocalDecls | Decl |
| AddiExpr | NodeWithChildren |
| RelOp | Node |
| AddOp | Node |
| MulOp | Node |
| Term | NodeWithChildren |
| Factor | Node |
| Call | NodeWithChildren|
| Variable | NodeWithChildren |
| Args | Node |

#### Terminal
| Class | Base class |
| --- | --- |
| Identifier | Node |
| SysType | Node |
| Integer | SysType |
| Float | SysType |
| Char | SysType |
| UnderScore | Variable |
| Void | Node |

#### Graph

```mermaid
graph TB
    Node-->NodeWithChildren
    Node-->TypeSpec
    Node-->Params
    Node-->RelOp
    Node-->AddOp
    Node-->MulOp
    Node-->Factor
    Node-->Args
    Node-->Identifier
    Node-->SysType
    Node-->Void
    NodeWithChildren-->Decl
    NodeWithChildren-->Param
    NodeWithChildren-->Statement
    NodeWithChildren-->SimpleExpr
    NodeWithChildren-->Term
    NodeWithChildren-->Call
    NodeWithChildren-->Variable
    Decl-->VarDecl
    Decl-->FuncDecl
    Decl-->LocalDecls
    Statement-->Stmts
    Statement-->ComStmt
    Statement-->IterStmt
    Statement-->SelectStmt
    Statement-->RetStmt
    Statement-->FuncStmt
    Statement-->ExprStmt
    SysType-->Integer
    SysType-->Float
    SysType-->Char
    Variable-->UnderScore
```
