#include "Lexer.hpp"
#include "Token.hpp"

void Lexer::tokenize(){
    while(source.at(position) != '\0'){
        nextToken();
    }
}
//"sdadadasdad"
Token Lexer::nextToken(){

    skipWhitespaceComments();
    if(isAtEnd()){
        return 1;
    }

    switch(getCurrChar()){
        
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
    return source.at(index);
}
bool Lexer::isAtEnd(){
    return(source.at(index) == '\0');
}