#include "Parser.hpp"

#include <iostream>
#include <utility>

Parser::Parser(
    const std::vector<Token>& tokens,
    SymbolTable& table
)
    : tokens(tokens),
      table_(table),
      current(0),
      hadError(false) {}

Token Parser::peek() const {
    return tokens.at(current);
}

Token Parser::previous() const {
    return current >= 1
        ? tokens.at(current - 1)
        : tokens.at(current);
}

bool Parser::isAtEnd() const {
    return tokens.at(current).type == TokenType::EndOfFile;
}

bool Parser::check(TokenType type) const {
    return tokens.at(current).type == type;
}

Token Parser::advance() {
    Token token = tokens.at(current);

    if (!isAtEnd()) {
        ++current;
    }

    return token;
}

bool Parser::match(TokenType type) {
    if (!check(type)) {
        return false;
    }

    advance();
    return true;
}

void Parser::expect(
    TokenType type,
    const std::string& message
) {
    if (check(type)) {
        advance();
        return;
    }

    error(peek(), message);
    throw ParseError{};
}

DataType Parser::expectDataType() {
    DataType type = toDataType(peek().type);

    if (type == DataType::Unknown) {
        error(
            peek(),
            "Se esperaba un tipo primitivo"
        );

        throw ParseError{};
    }

    advance();
    return type;
}

void Parser::error(
    const Token& token,
    const std::string& message
) {
    hadError = true;

    if (token.type == TokenType::Unknown) {
        return;
    }

    if (static_cast<int>(current) == lastErrorIndex) {
        return;
    }

    lastErrorIndex = static_cast<int>(current);

    if (token.type == TokenType::EndOfFile) {
        std::cerr
            << "error: al final del archivo: "
            << message
            << " (linea " << token.line
            << ", columna " << token.column << ")"
            << '\n';
    } else {
        std::cerr
            << "error: en '"
            << token.value
            << "': "
            << message
            << " (linea " << token.line
            << ", columna " << token.column << ")"
            << '\n';
    }
}

void Parser::skipToBlockEnd() {
    int depth = 0;

    while (!isAtEnd()) {
        if (check(TokenType::LBrace)) {
            ++depth;
        } else if (check(TokenType::RBrace)) {
            if (depth == 0) {
                return;
            }

            --depth;
        }

        advance();
    }
}

void Parser::skipToNextFunction() {
    while (!isAtEnd() && !check(TokenType::KwFn)) {
        advance();
    }
}

bool Parser::parse() {
    parseProgram();
    return !hadError;
}

void Parser::parseProgram() {
    while (!isAtEnd()) {
        try {
            std::unique_ptr<FunctionDeclNode> function =
                parseFunction();

            program_.functions.push_back(
                std::move(function)
            );
        } catch (const ParseError&) {
            skipToNextFunction();
        }
    }
}

std::unique_ptr<FunctionDeclNode>
Parser::parseFunction() {
    expect(
        TokenType::KwFn,
        "Se esperaba 'fn'"
    );

    expect(
        TokenType::Identifier,
        "Se esperaba el nombre de la funcion"
    );

    std::string functionName = previous().value;

    expect(
        TokenType::LParen,
        "Se esperaba '(' despues del nombre de la funcion"
    );

    std::vector<std::unique_ptr<ParameterNode>> parameters;

    if (!check(TokenType::RParen)) {
        do {
            expect(
                TokenType::Identifier,
                "Se esperaba el nombre del parametro"
            );

            std::string parameterName = previous().value;

            expect(
                TokenType::Colon,
                "Se esperaba ':' tras el nombre del parametro"
            );

            DataType parameterType = expectDataType();

            parameters.push_back(
                std::make_unique<ParameterNode>(
                    parameterName,
                    parameterType
                )
            );
        } while (match(TokenType::Comma));
    }

    expect(
        TokenType::RParen,
        "Se esperaba ')' al cerrar los parametros"
    );

    DataType returnType = DataType::Unknown;
    bool hasExplicitReturnType = false;

    if (match(TokenType::Arrow)) {
        returnType = expectDataType();
        hasExplicitReturnType = true;
    }

    expect(
        TokenType::LBrace,
        "Se esperaba '{' al inicio de un bloque"
    );

    // La firma ya es valida: la funcion y sus parametros entran a la tabla.
    table_.insert(functionName, returnType);

    for (const auto& parameter : parameters) {
        table_.insert(parameter->name, parameter->dataType);
    }

    std::unique_ptr<BlockStmtNode> body =
        parseBlockBody();

    return std::make_unique<FunctionDeclNode>(
        functionName,
        std::move(parameters),
        returnType,
        hasExplicitReturnType,
        std::move(body)
    );
}

std::unique_ptr<BlockStmtNode>
Parser::parseBlock() {
    expect(
        TokenType::LBrace,
        "Se esperaba '{' al inicio de un bloque"
    );

    return parseBlockBody();
}

// Parsea las sentencias de un bloque cuya '{' ya fue consumida.
std::unique_ptr<BlockStmtNode>
Parser::parseBlockBody() {
    auto block = std::make_unique<BlockStmtNode>();

    while (!check(TokenType::RBrace) && !isAtEnd()) {
        try {
            StmtPtr statement = parseStatement();

            block->statements.push_back(
                std::move(statement)
            );
        } catch (const ParseError&) {
            skipToBlockEnd();
        }
    }

    if (isAtEnd()) {
        error(
            peek(),
            "Se esperaba '}' al final de un bloque"
        );

        return block;
    }

    expect(
        TokenType::RBrace,
        "Se esperaba '}' al final de un bloque"
    );

    return block;
}

StmtPtr Parser::parseStatement() {
    if (check(TokenType::KwLet)) {
        return parseLetStatement();
    }

    if (check(TokenType::KwIf)) {
        return parseIfStatement();
    }

    if (check(TokenType::KwWhile)) {
        return parseWhileStatement();
    }

    if (check(TokenType::KwFor)) {
        return parseForStatement();
    }

    if (check(TokenType::KwReturn)) {
        return parseReturnStatement();
    }

    if (check(TokenType::LBrace)) {
        return parseBlock();
    }

    ExprPtr expression = parseExpression();

    expect(
        TokenType::Semicolon,
        "Se esperaba ';' despues de una expresion"
    );

    return std::make_unique<ExprStmtNode>(
        std::move(expression)
    );
}

StmtPtr Parser::parseLetStatement() {
    expect(
        TokenType::KwLet,
        "Se esperaba 'let' para declarar una variable"
    );

    expect(
        TokenType::Identifier,
        "Se esperaba el nombre de la variable"
    );

    std::string variableName = previous().value;

    DataType declaredType = DataType::Unknown;
    bool hasExplicitType = false;

    if (match(TokenType::Colon)) {
        declaredType = expectDataType();
        hasExplicitType = true;
    }

    ExprPtr initializer;

    if (match(TokenType::Equal)) {
        initializer = parseExpression();
    }

    expect(
        TokenType::Semicolon,
        "Se esperaba ';' al final de la declaracion let"
    );

    table_.insert(variableName, declaredType);

    return std::make_unique<LetStmtNode>(
        variableName,
        declaredType,
        hasExplicitType,
        std::move(initializer)
    );
}

StmtPtr Parser::parseIfStatement() {
    expect(
        TokenType::KwIf,
        "Se esperaba 'if' al inicio de una declaracion if"
    );

    ExprPtr condition = parseExpression();

    std::unique_ptr<BlockStmtNode> thenBranch =
        parseBlock();

    StmtPtr elseBranch;

    if (match(TokenType::KwElse)) {
        if (check(TokenType::KwIf)) {
            elseBranch = parseIfStatement();
        } else {
            std::unique_ptr<BlockStmtNode> elseBlock =
                parseBlock();

            elseBranch = std::move(elseBlock);
        }
    }

    return std::make_unique<IfStmtNode>(
        std::move(condition),
        std::move(thenBranch),
        std::move(elseBranch)
    );
}

StmtPtr Parser::parseWhileStatement() {
    expect(
        TokenType::KwWhile,
        "Se esperaba 'while' al inicio de un ciclo while"
    );

    ExprPtr condition = parseExpression();

    std::unique_ptr<BlockStmtNode> body =
        parseBlock();

    return std::make_unique<WhileStmtNode>(
        std::move(condition),
        std::move(body)
    );
}

StmtPtr Parser::parseForStatement() {
    expect(
        TokenType::KwFor,
        "Se esperaba 'for' al inicio de un ciclo for"
    );

    expect(
        TokenType::Identifier,
        "Se esperaba el nombre de la variable del ciclo"
    );

    std::string variableName = previous().value;

    expect(
        TokenType::KwIn,
        "Se esperaba 'in' despues del identificador"
    );

    ExprPtr iterable = parseExpression();

    expect(
        TokenType::LBrace,
        "Se esperaba '{' al inicio de un bloque"
    );

    //sin analisis semantico no se infiere el tipo de la variable del ciclo.
    table_.insert(variableName, DataType::Unknown);

    std::unique_ptr<BlockStmtNode> body =
        parseBlockBody();

    return std::make_unique<ForStmtNode>(
        variableName,
        std::move(iterable),
        std::move(body)
    );
}

StmtPtr Parser::parseReturnStatement() {
    expect(
        TokenType::KwReturn,
        "Se esperaba 'return'"
    );

    ExprPtr expression;

    if (!check(TokenType::Semicolon)) {
        expression = parseExpression();
    }

    expect(
        TokenType::Semicolon,
        "Se esperaba ';' al final del return"
    );

    return std::make_unique<ReturnStmtNode>(
        std::move(expression)
    );
}

ExprPtr Parser::parseExpression() {
    return parseAssignment();
}

ExprPtr Parser::parseAssignment() {
    ExprPtr left = parseLogicalOr();

    if (match(TokenType::Equal)) {
        ExprPtr right = parseAssignment();

        return std::make_unique<BinaryExprNode>(
            std::move(left),
            TokenType::Equal,
            std::move(right)
        );
    }

    return left;
}

ExprPtr Parser::parseLogicalOr() {
    ExprPtr expression = parseLogicalAnd();

    while (match(TokenType::OrOr)) {
        ExprPtr right = parseLogicalAnd();

        expression = std::make_unique<BinaryExprNode>(
            std::move(expression),
            TokenType::OrOr,
            std::move(right)
        );
    }

    return expression;
}

ExprPtr Parser::parseLogicalAnd() {
    ExprPtr expression = parseEquality();

    while (match(TokenType::AndAnd)) {
        ExprPtr right = parseEquality();

        expression = std::make_unique<BinaryExprNode>(
            std::move(expression),
            TokenType::AndAnd,
            std::move(right)
        );
    }

    return expression;
}

ExprPtr Parser::parseEquality() {
    ExprPtr expression = parseComparison();

    while (
        check(TokenType::EqualEqual) ||
        check(TokenType::NotEqual)
    ) {
        Token operatorToken = advance();

        ExprPtr right = parseComparison();

        expression = std::make_unique<BinaryExprNode>(
            std::move(expression),
            operatorToken.type,
            std::move(right)
        );
    }

    return expression;
}

ExprPtr Parser::parseComparison() {
    ExprPtr expression = parseRange();

    while (
        check(TokenType::Less) ||
        check(TokenType::LessEqual) ||
        check(TokenType::Greater) ||
        check(TokenType::GreaterEqual)
    ) {
        Token operatorToken = advance();

        ExprPtr right = parseRange();

        expression = std::make_unique<BinaryExprNode>(
            std::move(expression),
            operatorToken.type,
            std::move(right)
        );
    }

    return expression;
}

ExprPtr Parser::parseRange() {
    ExprPtr expression = parseTerm();

    while (match(TokenType::DotDot)) {
        ExprPtr right = parseTerm();

        expression = std::make_unique<BinaryExprNode>(
            std::move(expression),
            TokenType::DotDot,
            std::move(right)
        );
    }

    return expression;
}

ExprPtr Parser::parseTerm() {
    ExprPtr expression = parseFactor();

    while (
        check(TokenType::Plus) ||
        check(TokenType::Minus)
    ) {
        Token operatorToken = advance();

        ExprPtr right = parseFactor();

        expression = std::make_unique<BinaryExprNode>(
            std::move(expression),
            operatorToken.type,
            std::move(right)
        );
    }

    return expression;
}

ExprPtr Parser::parseFactor() {
    ExprPtr expression = parseUnary();

    while (
        check(TokenType::Star) ||
        check(TokenType::Slash)
    ) {
        Token operatorToken = advance();

        ExprPtr right = parseUnary();

        expression = std::make_unique<BinaryExprNode>(
            std::move(expression),
            operatorToken.type,
            std::move(right)
        );
    }

    return expression;
}

ExprPtr Parser::parseUnary() {
    if (
        check(TokenType::Not) ||
        check(TokenType::Minus)
    ) {
        Token operatorToken = advance();

        ExprPtr operand = parseUnary();

        return std::make_unique<UnaryExprNode>(
            operatorToken.type,
            std::move(operand)
        );
    }

    return parsePrimary();
}

ExprPtr Parser::parsePrimary() {
    if (
        check(TokenType::IntLiteral) ||
        check(TokenType::FloatLiteral) ||
        check(TokenType::StringLiteral) ||
        check(TokenType::CharLiteral) ||
        check(TokenType::BoolLiteral)
    ) {
        Token literalToken = advance();

        return std::make_unique<LiteralExprNode>(
            literalToken,
            literalDataType(literalToken.type)
        );
    }

    if (check(TokenType::Identifier)) {
        Token identifierToken = advance();

        if (match(TokenType::LParen)) {
            std::vector<ExprPtr> arguments;

            if (!check(TokenType::RParen)) {
                do {
                    arguments.push_back(
                        parseExpression()
                    );
                } while (match(TokenType::Comma));
            }

            expect(
                TokenType::RParen,
                "Se esperaba ')' al cerrar los argumentos"
            );

            return std::make_unique<CallExprNode>(
                identifierToken.value,
                std::move(arguments)
            );
        }

        return std::make_unique<IdentifierExprNode>(
            identifierToken.value
        );
    }

    if (match(TokenType::LParen)) {
        ExprPtr expression = parseExpression();

        expect(
            TokenType::RParen,
            "Se esperaba ')' tras la expresion"
        );

        return std::make_unique<GroupingExprNode>(
            std::move(expression)
        );
    }

    error(
        peek(),
        "Se esperaba una expresion valida"
    );

    throw ParseError{};
}