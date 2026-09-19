#ifndef EXPR_NODES_HPP
#define EXPR_NODES_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Ast.hpp"
#include "DataType.hpp"
#include "Token.hpp"

class LiteralExprNode : public ExprNode { //valores escritos literalmente
    public:
        LiteralExprNode(const Token& token, DataType datatype)
            : token(token), dataType(datatype) {}

        NodeKind kind() const override {
            return NodeKind::LiteralExpr;
        }

        Token token;
        DataType dataType;
};

class IdentifierExprNode : public ExprNode {//uso de una variable o identifier
public:
    explicit IdentifierExprNode(std::string name)
        : name(std::move(name)) {}

    NodeKind kind() const override {
        return NodeKind::IdentifierExpr;
    }

    std::string name;
};

class UnaryExprNode : public ExprNode {//operadores unarios
public:
    UnaryExprNode(TokenType operatorType, ExprPtr operand)
        : operatorType(operatorType), operand(std::move(operand)) {}

    NodeKind kind() const override {
        return NodeKind::UnaryExpr;
    }

    TokenType operatorType;
    ExprPtr operand;
};


class BinaryExprNode : public ExprNode {//operaciones con dos operandos
public:
    BinaryExprNode(
        ExprPtr left,
        TokenType operatorType,
        ExprPtr right
    )
        : left(std::move(left)),
          operatorType(operatorType),
          right(std::move(right)) {}

    NodeKind kind() const override {
        return NodeKind::BinaryExpr;
    }

    ExprPtr left;
    TokenType operatorType;
    ExprPtr right;
};

class CallExprNode : public ExprNode {//llamadas a funciones
public:
    CallExprNode(
        std::string functionName,
        std::vector<ExprPtr> arguments
    )
        : functionName(std::move(functionName)),
          arguments(std::move(arguments)) {}

    NodeKind kind() const override {
        return NodeKind::CallExpr;
    }

    std::string functionName;
    std::vector<ExprPtr> arguments;
};

class GroupingExprNode : public ExprNode {//funciones agrupadas por ()
public:
    explicit GroupingExprNode(ExprPtr expression)
        : expression(std::move(expression)) {}

    NodeKind kind() const override {
        return NodeKind::GroupingExpr;
    }

    ExprPtr expression;
};

#endif