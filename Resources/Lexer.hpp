#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>

#include "SymbolTable.hpp"
#include "Token.hpp"

// Analizador lexico: convierte el codigo fuente en una lista de tokens.
class Lexer {
public:
    Lexer(const std::string& source, SymbolTable& table);
    
    SymbolTable& table_;
    std::string source;

    // Posicion del caracter actual dentro de source.
    size_t index = 0;

    // Tokeniza todo el fuente; el ultimo token siempre es EndOfFile.
    std::vector<Token> tokenize();
    // Lee y devuelve el siguiente token.
    Token nextToken();

    // Errores lexicos encontrados.
    const std::vector<LexicalError>& errors() const { return errors_; }
    bool hadError = false;

private:

    std::vector<LexicalError> errors_;

    // ---- Recorrido del fuente ----
    void skipWhitespaceComments(); // Salta espacios, '//' y '/* */'
    void advance();// Avanza un caracter
    char getCurrChar() const;// Caracter actual ('\0' al final)
    char getNextChar() const;// Caracter siguiente ('\0' al final)
    bool isAtEnd() const;

    // ---- Clasificacion de caracteres ----
    static bool isDigit(char c);
    static bool isIdentifier(char c);// Letra o '_'
    static bool isValidEscape(char c); // Caracter valido despues de '\'

    // skips: cuantos caracteres consume el token antes de registrarse.
    Token makeToken(TokenType type, const std::string& lexeme, int skips = 0);
    // Registra un error lexico y devuelve un token Unknown.
    Token makeUnknown(const std::string& message, const std::string& lexeme);

    // ---- Tokens de varios caracteres ----
    Token readNumber();     // Entero o float
    Token readIdentifier(); // Identificador, palabra reservada, tipo o bool
    Token readString();                 // "..."
    Token readChar();                   // 'c'
    
};
#endif
