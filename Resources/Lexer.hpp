#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

#include "Token.hpp"

class Lexer {
public:
    explicit Lexer(const std::string& source);

    std::string source;

    size_t index = 0;

    int line = 1;
    int column = 1;

    std::vector<Token> tokenize();
    Token nextToken();

    const std::vector<LexicalError>& errors() const { return errors_; }
    bool hadError = false;

private:

    // Posicion donde empieza el token que se esta construyendo.
    int tokenLine = 1;
    int tokenColumn = 1;
    std::vector<LexicalError> errors_;

    void skipWhitespaceComments();
    void advance();
    char getCurrChar() const;
    char getNextChar() const;
    bool isAtEnd() const;
    static bool isDigit(char c);
    static bool isIdentifier(char c);
    static bool isValidEscape(char c);

    // skips: cuantos caracteres consume el token antes de registrarse.
    Token makeToken(TokenType type, const std::string& lexeme, int skips = 0);
    // Registra un error lexico y devuelve un token Unknown.
    Token makeUnknown(const std::string& message, const std::string& lexeme);

    Token readNumber(size_t start);
    Token readIdentifier(size_t start);
    Token readString();
    Token readChar();
    
};
#endif
