#ifndef STMT_NODES_HPP
#define STMT_NODES_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Ast.hpp"
#include "DataType.hpp"
#include "ExprNodes.hpp"

class LetStmtNode : public StmtNode {
public:
    LetStmtNode(
        std::string name,
        DataType declaredType,
        bool hasExplicitType,
        ExprPtr initializer
    )
        : name(std::move(name)),
          declaredType(declaredType),
          hasExplicitType(hasExplicitType),
          initializer(std::move(initializer)) {}

    NodeKind kind() const override {
        return NodeKind::LetStmt;
    }

    std::string name;
    DataType declaredType;
    bool hasExplicitType;
    ExprPtr initializer;
};

class ExprStmtNode : public StmtNode {
public:
    explicit ExprStmtNode(ExprPtr expression)
        : expression(std::move(expression)) {}

    NodeKind kind() const override {
        return NodeKind::ExprStmt;
    }

    ExprPtr expression;
};

class ReturnStmtNode : public StmtNode {
public:
    explicit ReturnStmtNode(ExprPtr expression)
        : expression(std::move(expression)) {}

    NodeKind kind() const override {
        return NodeKind::ReturnStmt;
    }

    ExprPtr expression;
};

class BlockStmtNode : public StmtNode {
public:
    BlockStmtNode() = default;

    NodeKind kind() const override {
        return NodeKind::BlockStmt;
    }

    StmtList statements;
};

class IfStmtNode : public StmtNode {
public:
    IfStmtNode(
        ExprPtr condition,
        std::unique_ptr<BlockStmtNode> thenBranch,
        StmtPtr elseBranch
    )
        : condition(std::move(condition)),
          thenBranch(std::move(thenBranch)),
          elseBranch(std::move(elseBranch)) {}

    NodeKind kind() const override {
        return NodeKind::IfStmt;
    }

    ExprPtr condition;
    std::unique_ptr<BlockStmtNode> thenBranch;
    StmtPtr elseBranch;
};

class WhileStmtNode : public StmtNode {
public:
    WhileStmtNode(
        ExprPtr condition,
        std::unique_ptr<BlockStmtNode> body
    )
        : condition(std::move(condition)),
          body(std::move(body)) {}

    NodeKind kind() const override {
        return NodeKind::WhileStmt;
    }

    ExprPtr condition;
    std::unique_ptr<BlockStmtNode> body;
};

class ForStmtNode : public StmtNode {
public:
    ForStmtNode(
        std::string variableName,
        ExprPtr iterable,
        std::unique_ptr<BlockStmtNode> body
    )
        : variableName(std::move(variableName)),
          iterable(std::move(iterable)),
          body(std::move(body)) {}

    NodeKind kind() const override {
        return NodeKind::ForStmt;
    }

    std::string variableName;
    ExprPtr iterable;
    std::unique_ptr<BlockStmtNode> body;
};

#endif 