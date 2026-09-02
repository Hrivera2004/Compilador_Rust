#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <fstream>
#include <string>

class Lexer {
public:
    explicit Lexer(const std::string& source);

    std::string source;

    size_t index = 0;


    int line = 0 ;
    int column = 0;

    void tokenize();
    Token nextToken();
    
private:
    
    void skipWhitespaceComments();
    void advance();
    char getCurrChar();
    bool isAtEnd();
    bool isDigit(char c);
    bool isABC_(char c);
    Token makeToken();
    Token readNumber();
    Token readString();
};
#endif