#ifndef AST_HPP
#define AST_HPP

#include <memory>
#include <vector>

enum class NodeKind {//tipo especifico de nodo
    Program,
    FunctionDecl,
    Parameter,
    BlockStmt,
    LetStmt,
    ExprStmt,
    ReturnStmt,
    IfStmt,
    WhileStmt,
    ForStmt,
    LiteralExpr,
    IdentifierExpr,
    UnaryExpr,
    BinaryExpr,
    CallExpr,
    GroupingExpr
};

class AstNode{ // representa cualquier nodo
    public:
        virtual ~AstNode() = default;
        virtual NodeKind kind() const = 0;
};

class ExprNode : public AstNode {//representa expressiones
    public:
        ~ExprNode() override = default;
};

class StmtNode : public AstNode {//representa sentencias
    public:
        ~StmtNode() override = default;
};

using ExprPtr = std::unique_ptr<ExprNode>;
using StmtPtr = std::unique_ptr<StmtNode>;
using StmtList = std::vector<StmtPtr>;

#endif