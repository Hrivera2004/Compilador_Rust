#ifndef DECL_NODES_HPP
#define DECL_NODES_HPP

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "Ast.hpp"
#include "DataType.hpp"
#include "StmtNodes.hpp"

class ParameterNode : public AstNode {//para el tipo en los parametros de funciones
public:
    ParameterNode(
        std::string name,
        DataType dataType
    )
        : name(std::move(name)),
          dataType(dataType) {}

    NodeKind kind() const override {
        return NodeKind::Parameter;
    }

    std::string name;
    DataType dataType;
};

class FunctionDeclNode : public AstNode {
public:
    FunctionDeclNode(
        std::string name,
        std::vector<std::unique_ptr<ParameterNode>> parameters,
        DataType returnType,
        bool hasExplicitReturnType,
        std::unique_ptr<BlockStmtNode> body
    )
        : name(std::move(name)),
          parameters(std::move(parameters)),
          returnType(returnType),
          hasExplicitReturnType(hasExplicitReturnType),
          body(std::move(body)) {}

    NodeKind kind() const override {
        return NodeKind::FunctionDecl;
    }

    std::string name;
    std::vector<std::unique_ptr<ParameterNode>> parameters;
    DataType returnType;
    bool hasExplicitReturnType;
    std::unique_ptr<BlockStmtNode> body;
};

class ProgramNode : public AstNode {
public:
    ProgramNode() = default;

    NodeKind kind() const override {
        return NodeKind::Program;
    }

    std::vector<std::unique_ptr<FunctionDeclNode>> functions;
};

#endif 