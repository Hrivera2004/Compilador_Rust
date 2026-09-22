#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

/// Categoria lexica de un token reconocido por el lexer.

/// Alcance del lenguaje:
/// - Identificadores y palabras reservadas (let, fn, if, else, while, for, in, return)
/// - Tipos de datos primitivos (i32, f64, bool, char, str)
/// - Operadores aritmeticos, logicos y de comparacion (+, -, *, /, &&, ||, !, ==, !=, <, <=, >, >=)
/// - Asignacion, retorno y rango (=, ->, ..)
/// - Delimitadores y simbolos ({}, (), [], ,, ;, :)
/// - Expresiones y sentencias de control de flujo (if-else, while, for)
/// - Definicion de funciones con un numero limitado de parametros

enum class TokenType {
    // ---- Identificadores y literales ----
    Identifier,//
    IntLiteral,//
    FloatLiteral,//
    StringLiteral,
    CharLiteral,
    BoolLiteral,//

    // ---- Palabras reservadas ----
    KwLet, KwFn, KwIf, KwElse, KwWhile, KwFor, KwReturn, KwIn, //

    // ---- Tipos primitivos ----
    TypeI32, TypeF64, TypeBool, TypeChar, TypeStr, //

    // ---- Operadores ----
    Plus, Minus, Star, Slash,
    AndAnd, OrOr, Not,
    EqualEqual, NotEqual, Less, LessEqual, Greater, GreaterEqual,
    Equal, Arrow, DotDot,

    // ---- Delimitadores ----
    LBrace, RBrace,     
    LParen, RParen,     
    LBracket, RBracket, 
    Comma, Semicolon, Colon,

    // ---- Especiales ----
    EndOfFile,// Marca el final del fuente
    Unknown// Token invalido; su error queda en Lexer::errors()
};

// Struct principal del token.
struct Token {
    TokenType type;
    std::string value; // Lexema tal como aparece en el fuente
    int line;
    int column;
};

// Error lexico reportado por el lexer.
struct LexicalError {
    std::string message;
    int line;
    int column;
};

#endif 
