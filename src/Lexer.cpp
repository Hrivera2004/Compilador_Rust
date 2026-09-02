#include "Lexer.hpp"
#include "Token.hpp"

void Lexer::tokenize(){
    while(source.at(position) != '\0'){
        nextToken();
    }
}

Token Lexer::nextToken(){
    skipWhitespaceComments();
    char c = getCurrChar();

    if(isAtEnd(c)){
        return 1;
    }
    if (isDigit(c))
        return readNumber(startLine, startColumn);
    if (isIdentifier(c))
        return readNumber(startLine, startColumn);

    switch(getCurrChar(c)){
        
    }


    Token Tc = {TokenType, Valor, line, column}
    return Tc;
}

void Lexer::skipWhitespaceComments() {
    while(index < source.lenght()){
        char current = getCurrChar();

        // para espacios en blanco y saltos de linea
        if((current == ' ') || (current == '\t') || (current == '\r') || (current == '\n') ){
            advance();
        } else if ((current == '/' ) && (index + 1 < source.length)){
            char next = source.at(index+1);

            //es comentario de una linea?
            if(next == '/') {
                advance();
                advance();
                //comentario multilinea
                while((index < source.lenght()) && (source.at(index) != '\n')) {
                    advance();
                }
            } else if (next == '*') {
                advance();
                advance();
                while(index < source.length()){
                    if ((source.at(index) == '*') && (index + 1 < source.length()) && (source.at(index + 1) == '/')){
                        advance();
                        advance();
                        break;
                    }
                    advance();
                }
            } else {
                return;
                //solo es ope de division
            }
        } else {
            return;
            //es caracter valido
        }
    }
}

void Lexer::advance(){
    if(isAtEnd()) return 1;
    index++;
    if(source.at(index) =='\n'){
        line++;
        column=0;
    }else{
        column++;
    }
}

char Lexer::getCurrChar(){
    if (index >= source.size()) return '\0';
    return source.at(index);
}
char Lexer::getNextChar(){
    if (index + 1 >= source.size()) return '\0';
    return source.at(index + 1);
}
bool Lexer::isAtEnd(char c){
    return(c == '\0');
}

bool Lexer::isDigit(char c){
    return (c >= '0' && c <= '9');
}

bool Lexer::isIdentifier(char c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
Token Lexer::makeToken(TokenType type, const std::string& lexeme) const{
        return Token{type, lexeme, line, column};
}
Token Lexer::readNumber(int start){
    while(isDigit(getCurrChar())){
        advance();

    }
    if(getCurrChar() == '.' && isDigit(getNextChar())) {
        while(isDigit(getCurrChar())){
            advance();
        }
        lexeme.substr(start, index, source);
        return makeToken(TokenType::FloatLiteral, lexeme);
    }
    std::string lexeme = source.substr(start, index, source);
    return makeToken(IntLiteral, lexeme);
    
}
Token Lexer::readIdentifier(int start){
    while(isIdentifier(getCurrChar())){
        advance();
    }
    std::string lexeme = source.substr(start, index, source);
    return makeToken(TokenType::Identifier, lexeme);
}

