#ifndef PARSER_HPP
#define PARSER_HPP

#include "Token.hpp"
#include <vector>
#include <string>
#include <iostream>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    bool parse();

private:
    //Attributes
    std::vector<Token> tokens;
    size_t current = 0;
    bool hadError = false;

    //A-Methods
    Token peek() const;
    Token previous() const;
    bool isAtEnd() const;
    bool check(TokenType type) const; 
    Token advance(); 
    bool match(TokenType type);
    void expect(TokenType type, const std::string& message);
    void error(const Token& token, const std::string& message);
    void synchronize();

    //G-Methods
    void parseProgram();
    void parseFunction();
    void parseBlock();
    void parseStatement();
    void parseLetStatement();
    void parseIfStatement();
    void parseWhileStatement();
    void parseReturnStatement();
    void parseExpression();

};


#endif