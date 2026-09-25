#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

#include "Token.hpp"

//convierte el codigo fuente en una lista de tokens.
class Lexer {
public:
    Lexer(const std::string& source);

    std::string source;

    size_t index = 0;

    int line = 1;
    int column = 1;

    std::vector<Token> tokenize();
    Token nextToken();

    const std::vector<LexicalError>& errors() const { return errors_; }
    bool hadError = false;

private:
    int tokenLine = 1;
    int tokenColumn = 1;
    std::vector<LexicalError> errors_;

    void skipWhitespaceComments(); 
    void advance();
    char getCurrChar() const;
    char getNextChar() const;
    bool isAtEnd() const;

    //clasificacion de caracteres
    static bool isDigit(char c);
    static bool isIdentifier(char c);
    static bool isValidEscape(char c);

    // skips: cuantos caracteres consume el token antes de registrarse.
    Token makeToken(TokenType type, const std::string& lexeme, int skips = 0);
    Token makeUnknown(const std::string& message, const std::string& lexeme);

    Token readNumber();
    Token readIdentifier();
    Token readString();
    Token readChar();
    
};
#endif
