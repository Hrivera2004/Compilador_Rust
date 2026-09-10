#include "Parser.hpp"

Token Parser::peek() const{
    return tokens.at(current);
}
Token Parser::previous() const{
    return (current >= 1? tokens.at(current - 1):  tokens.at(current));
}


bool Parser::isAtEnd() const{
    return tokens.at(current).type == TokenType::EndOfFile;
}
bool Parser::check(TokenType type) const{
    TokenType TT = tokens.at(current).type;
    return (TT == type);
} 
    
Token Parser::advance(){
    Token T = tokens.at(current);
    if(!isAtEnd()) current++;
    return T;
} 
bool Parser::match(TokenType type){
    if(!check(type)) return false;
    advance();
    return true;
}

void Parser::expect(TokenType type, const std::string& message){
    if(check(type)){
        advance();
        return;
    }
    error(peek(), message);
    throw ParseError{};
}

void Parser::error(const Token& token, const std::string& message){
    hadError = true;
    if(token.type == TokenType::EndOfFile)
        std::cerr << token.line << ":" << token.columna
        << ": error: al final del archivo: " << message << "\n";
    else
        std::cerr << token.line << ":" << token.column
        << ": error: en '" << token.value  << "': " << message << "\n";
}

void Parser::synchronize(){
    advance();
     while(!isAtEnd()){
        
        if(previous().type == TokenType::Semicolon) return;
        switch(peek().type){
            case TokenType::KwFn:
            case TokenType::KwLet:
            case TokenType::KwIf:
            case TokenType::KwWhile:
            case TokenType::KwFor:
            case TokenType::KwReturn:
            case TokenType::RBrace:
                return;
            default:
                break;
        }
        advance();
    }
}

void Parser::parseProgram(){
    while(!isAtEnd()){
        parseFunction();
    }
}

void Parser::parseFunction(){
    
}

void Parser::parseBlock(){
    
}

void Parser::parseStatement(){
    
}

void Parser::parseLetStatement(){
    
}

void Parser::parseIfStatement(){
    
}

void Parser::parseWhileStatement(){
    
}

void Parser::parseReturnStatement(){
    
}

void Parser::parseExpression(){
    
}