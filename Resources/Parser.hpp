#ifndef PARSER_HPP
#define PARSER_HPP

#include "Token.hpp"
#include <vector>
#include <string>
#include <iostream>

// Analizador sintactico descendente recursivo.
// Solo valida la estructura; no construye un arbol.
class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    // Analiza todos los tokens; devuelve true si no hubo errores.
    bool parse();

private:
    //Attributes
    std::vector<Token> tokens;
    size_t current = 0;    // Indice del token actual
    bool hadError = false;
    struct ParseError {};  // Se lanza para abortar la regla actual y recuperarse

    // Indice del token del ultimo error reportado, para no repetirlo.
    int lastErrorIndex = -1;


    //A-Methods (auxiliares)
    Token peek() const;                // Token actual
    Token previous() const;            // Token anterior
    bool isAtEnd() const;              // Token actual es EndOfFile
    bool check(TokenType type) const;  // Token actual es de ese tipo (no consume)

    Token advance();                   // Consume el token actual
    bool match(TokenType type);        // Consume si es de ese tipo
    void expect(TokenType type, const std::string& message); // Consume o lanza ParseError

    void error(const Token& token, const std::string& message); // Reporta un error
    // Recuperacion de errores
    void skipToBlockEnd();
    void skipToNextFunction();

    //G-Methods (una funcion por regla de la gramatica)
    void parseProgram();
    void parseFunction();
    void parseBlock();
    void parseStatement();
    void parseLetStatement();
    void parseIfStatement();
    void parseWhileStatement();
    void parseForStatement();
    void parseReturnStatement();
    void parseExpression();

};


#endif
