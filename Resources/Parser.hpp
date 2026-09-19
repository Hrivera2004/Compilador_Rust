#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>
#include <string>
#include <vector>

#include "DeclNodes.hpp"
#include "SymbolTable.hpp"
#include "Token.hpp"

// Parser descendente recursivo que valida la gramatica
// y construye el AST del programa.
class Parser {
public:
    Parser(
        const std::vector<Token>& tokens,
        SymbolTable& table
    );

    bool parse();

    const ProgramNode& ast() const {
        return program_;
    }

private:
    std::vector<Token> tokens;
    SymbolTable& table_;

    // El parser construye el arbol completo en este nodo raiz.
    ProgramNode program_;

    size_t current = 0;
    bool hadError = false;

    struct ParseError {};

    // Evita reportar varias veces el mismo error sintactico.
    int lastErrorIndex = -1;

    Token peek() const;
    Token previous() const;
    bool isAtEnd() const;
    bool check(TokenType type) const;

    Token advance();
    bool match(TokenType type);

    void expect(
        TokenType type,
        const std::string& message
    );

    DataType expectDataType();

    void error(
        const Token& token,
        const std::string& message
    );

    // Recuperacion para continuar analizando otras funciones o bloques.
    void skipToBlockEnd();
    void skipToNextFunction();

    void parseProgram();

    std::unique_ptr<FunctionDeclNode> parseFunction();

    std::unique_ptr<BlockStmtNode> parseBlock();

    StmtPtr parseStatement();
    StmtPtr parseLetStatement();
    StmtPtr parseIfStatement();
    StmtPtr parseWhileStatement();
    StmtPtr parseForStatement();
    StmtPtr parseReturnStatement();

    ExprPtr parseExpression();

    ExprPtr parseAssignment();
    ExprPtr parseLogicalOr();
    ExprPtr parseLogicalAnd();
    ExprPtr parseEquality();
    ExprPtr parseComparison();
    ExprPtr parseRange();
    ExprPtr parseTerm();
    ExprPtr parseFactor();
    ExprPtr parseUnary();
    ExprPtr parsePrimary();
};

#endif // PARSER_HPP