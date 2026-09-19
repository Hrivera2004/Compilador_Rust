#ifndef AST_PRINTER_HPP
#define AST_PRINTER_HPP

#include <ostream>
#include <string>

#include "DeclNodes.hpp"

class AstPrinter {
public:
    void print(const ProgramNode& program, std::ostream& out) const;

private:
    void printFunction(
        const FunctionDeclNode& function,
        const std::string& prefix,
        bool isLast,
        std::ostream& out
    ) const;

    void printStatement(
        const StmtNode& statement,
        const std::string& prefix,
        bool isLast,
        std::ostream& out
    ) const;

    void printExpression(
        const ExprNode& expression,
        const std::string& prefix,
        bool isLast,
        std::ostream& out
    ) const;

    void printBranch(
        const std::string& prefix,
        bool isLast,
        const std::string& label,
        std::ostream& out
    ) const;

    std::string childPrefix(
        const std::string& prefix,
        bool isLast
    ) const;
};

#endif 