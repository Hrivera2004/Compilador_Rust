#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>
#include <fstream>
#include <string>

class Lexer {
public:
    explicit Lexer(const std::string& source);

    String source;

    size_t index = 0;

    
    int line = 0 ;
    int column = 0;

    void tokenize();
    Token nextToke();
    
private:
    
    void skipWhitespaceComments();
    void advance();
    char getCurrChar();
    bool isAtEnd();
};
#endif