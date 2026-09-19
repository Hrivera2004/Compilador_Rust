#include "AstPrinter.hpp"

#include <string>

namespace {

std::string operatorName(TokenType type) {
    switch (type) {
        case TokenType::Plus:
            return "+";
        case TokenType::Minus:
            return "-";
        case TokenType::Star:
            return "*";
        case TokenType::Slash:
            return "/";
        case TokenType::AndAnd:
            return "&&";
        case TokenType::OrOr:
            return "||";
        case TokenType::Not:
            return "!";
        case TokenType::Equal:
            return "=";
        case TokenType::EqualEqual:
            return "==";
        case TokenType::NotEqual:
            return "!=";
        case TokenType::Less:
            return "<";
        case TokenType::LessEqual:
            return "<=";
        case TokenType::Greater:
            return ">";
        case TokenType::GreaterEqual:
            return ">=";
        case TokenType::DotDot:
            return "..";
        default:
            return "?";
    }
}

std::string literalName(TokenType type) {
    switch (type) {
        case TokenType::IntLiteral:
            return "IntLiteral";
        case TokenType::FloatLiteral:
            return "FloatLiteral";
        case TokenType::BoolLiteral:
            return "BoolLiteral";
        case TokenType::CharLiteral:
            return "CharLiteral";
        case TokenType::StringLiteral:
            return "StringLiteral";
        default:
            return "Literal";
    }
}

}

void AstPrinter::print(
    const ProgramNode& program,
    std::ostream& out
) const {
    out << "Program\n";

    for (size_t i = 0; i < program.functions.size(); ++i) {
        bool isLast = i + 1 == program.functions.size();

        printFunction(
            *program.functions[i],
            "",
            isLast,
            out
        );
    }
}

void AstPrinter::printFunction(
    const FunctionDeclNode& function,
    const std::string& prefix,
    bool isLast,
    std::ostream& out
) const {
    printBranch(
        prefix,
        isLast,
        "FunctionDecl: " + function.name,
        out
    );

    std::string nextPrefix =
        childPrefix(prefix, isLast);

    int childCount = 1;

    if (!function.parameters.empty()) {
        ++childCount;
    }

    if (function.hasExplicitReturnType) {
        ++childCount;
    }

    int childIndex = 0;

    if (!function.parameters.empty()) {
        ++childIndex;

        bool parametersAreLast =
            childIndex == childCount;

        printBranch(
            nextPrefix,
            parametersAreLast,
            "Parameters",
            out
        );

        std::string parametersPrefix =
            childPrefix(nextPrefix, parametersAreLast);

        for (size_t i = 0;
             i < function.parameters.size();
             ++i) {
            bool parameterIsLast =
                i + 1 == function.parameters.size();

            const ParameterNode& parameter =
                *function.parameters[i];

            printBranch(
                parametersPrefix,
                parameterIsLast,
                "id: " + parameter.name +
                    " : " + toString(parameter.dataType),
                out
            );
        }
    }

    if (function.hasExplicitReturnType) {
        ++childIndex;

        bool returnTypeIsLast =
            childIndex == childCount;

        printBranch(
            nextPrefix,
            returnTypeIsLast,
            "return type: " +
                toString(function.returnType),
            out
        );
    }

    ++childIndex;

    printStatement(
        *function.body,
        nextPrefix,
        childIndex == childCount,
        out
    );
}

void AstPrinter::printStatement(
    const StmtNode& statement,
    const std::string& prefix,
    bool isLast,
    std::ostream& out
) const {
    if (statement.kind() == NodeKind::BlockStmt) {
        const auto& block =
            static_cast<const BlockStmtNode&>(statement);

        printBranch(
            prefix,
            isLast,
            "BlockStmt",
            out
        );

        std::string nextPrefix =
            childPrefix(prefix, isLast);

        for (size_t i = 0;
             i < block.statements.size();
             ++i) {
            bool childIsLast =
                i + 1 == block.statements.size();

            printStatement(
                *block.statements[i],
                nextPrefix,
                childIsLast,
                out
            );
        }

        return;
    }

    if (statement.kind() == NodeKind::LetStmt) {
        const auto& letStatement =
            static_cast<const LetStmtNode&>(statement);

        printBranch(
            prefix,
            isLast,
            "LetStmt",
            out
        );

        std::string nextPrefix =
            childPrefix(prefix, isLast);

        bool hasType =
            letStatement.hasExplicitType;

        bool hasInitializer =
            letStatement.initializer != nullptr;

        int childCount = 1;

        if (hasType) {
            ++childCount;
        }

        if (hasInitializer) {
            ++childCount;
        }

        int childIndex = 0;

        ++childIndex;

        printBranch(
            nextPrefix,
            childIndex == childCount,
            "id: " + letStatement.name,
            out
        );

        if (hasType) {
            ++childIndex;

            printBranch(
                nextPrefix,
                childIndex == childCount,
                "type: " +
                    toString(letStatement.declaredType),
                out
            );
        }

        if (hasInitializer) {
            ++childIndex;

            bool initializerIsLast =
                childIndex == childCount;

            printBranch(
                nextPrefix,
                initializerIsLast,
                "initializer",
                out
            );

            printExpression(
                *letStatement.initializer,
                childPrefix(
                    nextPrefix,
                    initializerIsLast
                ),
                true,
                out
            );
        }

        return;
    }

    if (statement.kind() == NodeKind::ExprStmt) {
        const auto& expressionStatement =
            static_cast<const ExprStmtNode&>(statement);

        printBranch(
            prefix,
            isLast,
            "ExprStmt",
            out
        );

        printExpression(
            *expressionStatement.expression,
            childPrefix(prefix, isLast),
            true,
            out
        );

        return;
    }

    if (statement.kind() == NodeKind::ReturnStmt) {
        const auto& returnStatement =
            static_cast<const ReturnStmtNode&>(statement);

        printBranch(
            prefix,
            isLast,
            "ReturnStmt",
            out
        );

        if (returnStatement.expression != nullptr) {
            printExpression(
                *returnStatement.expression,
                childPrefix(prefix, isLast),
                true,
                out
            );
        }

        return;
    }

    if (statement.kind() == NodeKind::IfStmt) {
        const auto& ifStatement =
            static_cast<const IfStmtNode&>(statement);

        printBranch(
            prefix,
            isLast,
            "IfStmt",
            out
        );

        std::string nextPrefix =
            childPrefix(prefix, isLast);

        bool hasElse =
            ifStatement.elseBranch != nullptr;

        int childCount = hasElse ? 3 : 2;
        int childIndex = 0;

        ++childIndex;

        printBranch(
            nextPrefix,
            false,
            "condition",
            out
        );

        printExpression(
            *ifStatement.condition,
            childPrefix(nextPrefix, false),
            true,
            out
        );

        ++childIndex;

        bool thenIsLast =
            childIndex == childCount;

        printBranch(
            nextPrefix,
            thenIsLast,
            "then",
            out
        );

        printStatement(
            *ifStatement.thenBranch,
            childPrefix(nextPrefix, thenIsLast),
            true,
            out
        );

        if (hasElse) {
            printBranch(
                nextPrefix,
                true,
                "else",
                out
            );

            printStatement(
                *ifStatement.elseBranch,
                childPrefix(nextPrefix, true),
                true,
                out
            );
        }

        return;
    }

    if (statement.kind() == NodeKind::WhileStmt) {
        const auto& whileStatement =
            static_cast<const WhileStmtNode&>(statement);

        printBranch(
            prefix,
            isLast,
            "WhileStmt",
            out
        );

        std::string nextPrefix =
            childPrefix(prefix, isLast);

        printBranch(
            nextPrefix,
            false,
            "condition",
            out
        );

        printExpression(
            *whileStatement.condition,
            childPrefix(nextPrefix, false),
            true,
            out
        );

        printBranch(
            nextPrefix,
            true,
            "body",
            out
        );

        printStatement(
            *whileStatement.body,
            childPrefix(nextPrefix, true),
            true,
            out
        );

        return;
    }

    if (statement.kind() == NodeKind::ForStmt) {
        const auto& forStatement =
            static_cast<const ForStmtNode&>(statement);

        printBranch(
            prefix,
            isLast,
            "ForStmt",
            out
        );

        std::string nextPrefix =
            childPrefix(prefix, isLast);

        printBranch(
            nextPrefix,
            false,
            "id: " + forStatement.variableName,
            out
        );

        printBranch(
            nextPrefix,
            false,
            "iterable",
            out
        );

        printExpression(
            *forStatement.iterable,
            childPrefix(nextPrefix, false),
            true,
            out
        );

        printBranch(
            nextPrefix,
            true,
            "body",
            out
        );

        printStatement(
            *forStatement.body,
            childPrefix(nextPrefix, true),
            true,
            out
        );

        return;
    }
}

void AstPrinter::printExpression(
    const ExprNode& expression,
    const std::string& prefix,
    bool isLast,
    std::ostream& out
) const {
    if (expression.kind() == NodeKind::LiteralExpr) {
        const auto& literal =
            static_cast<const LiteralExprNode&>(expression);

        printBranch(
            prefix,
            isLast,
            literalName(literal.token.type) +
                ": " + literal.token.value,
            out
        );

        return;
    }

    if (expression.kind() == NodeKind::IdentifierExpr) {
        const auto& identifier =
            static_cast<const IdentifierExprNode&>(expression);

        printBranch(
            prefix,
            isLast,
            "Identifier: " + identifier.name,
            out
        );

        return;
    }

    if (expression.kind() == NodeKind::BinaryExpr) {
        const auto& binary =
            static_cast<const BinaryExprNode&>(expression);

        printBranch(
            prefix,
            isLast,
            "BinaryExpr: " +
                operatorName(binary.operatorType),
            out
        );

        std::string nextPrefix =
            childPrefix(prefix, isLast);

        printExpression(
            *binary.left,
            nextPrefix,
            false,
            out
        );

        printExpression(
            *binary.right,
            nextPrefix,
            true,
            out
        );

        return;
    }

    if (expression.kind() == NodeKind::UnaryExpr) {
        const auto& unary =
            static_cast<const UnaryExprNode&>(expression);

        printBranch(
            prefix,
            isLast,
            "UnaryExpr: " +
                operatorName(unary.operatorType),
            out
        );

        printExpression(
            *unary.operand,
            childPrefix(prefix, isLast),
            true,
            out
        );

        return;
    }

    if (expression.kind() == NodeKind::GroupingExpr) {
        const auto& grouping =
            static_cast<const GroupingExprNode&>(expression);

        printBranch(
            prefix,
            isLast,
            "GroupingExpr",
            out
        );

        printExpression(
            *grouping.expression,
            childPrefix(prefix, isLast),
            true,
            out
        );

        return;
    }

    if (expression.kind() == NodeKind::CallExpr) {
        const auto& call =
            static_cast<const CallExprNode&>(expression);

        printBranch(
            prefix,
            isLast,
            "CallExpr: " + call.functionName,
            out
        );

        std::string nextPrefix =
            childPrefix(prefix, isLast);

        for (size_t i = 0;
             i < call.arguments.size();
             ++i) {
            bool argumentIsLast =
                i + 1 == call.arguments.size();

            printExpression(
                *call.arguments[i],
                nextPrefix,
                argumentIsLast,
                out
            );
        }

        return;
    }
}

void AstPrinter::printBranch(
    const std::string& prefix,
    bool isLast,
    const std::string& label,
    std::ostream& out
) const {
    out << prefix
        << (isLast ? "\\-- " : "+-- ")
        << label
        << '\n';
}

std::string AstPrinter::childPrefix(
    const std::string& prefix,
    bool isLast
) const {
    return prefix + (isLast ? "    " : "|   ");
}