#include "Lexer.hpp"

#include <unordered_map>

Lexer::Lexer(const std::string& source) : source(source) {}

std::vector<Token> Lexer::tokenize(){
    std::vector<Token> tokens;
    while(true){
        Token t = nextToken();
        tokens.push_back(t);
        if(t.type == TokenType::EndOfFile) break;
    }
    return tokens;
}

Token Lexer::nextToken(){
    skipWhitespaceComments();

    tokenLine = line;
    tokenColumn = column;

    if(isAtEnd())
        return makeToken(TokenType::EndOfFile, "");

    size_t start = index;
    char c = getCurrChar();

    if (isDigit(c))
        return readNumber(start);

    if (isIdentifier(c))
        return readIdentifier(start);

    switch(c){
        // ---- Delimitadores (un caracter, sin lookahead) ----
        case '{': return makeSingle(TokenType::LBrace, c, 1);
        case '}': return makeSingle(TokenType::RBrace, c, 1);
        case '(': return makeSingle(TokenType::LParen, c, 1);
        case ')': return makeSingle(TokenType::RParen, c, 1);
        case '[': return makeSingle(TokenType::LBracket, c, 1);
        case ']': return makeSingle(TokenType::RBracket, c, 1);
        case ',': return makeSingle(TokenType::Comma, c, 1);
        case ';': return makeSingle(TokenType::Semicolon, c, 1);
        case ':': return makeSingle(TokenType::Colon, c, 1);

        // ---- Operadores simples ----
        case '+': return makeSingle(TokenType::Plus, c, 1);
        case '*': return makeSingle(TokenType::Star, c, 1);

        // ---- Lookahead con fallback valido ----
        case '=':
            if (getNextChar() == '='){
                    return makeToken(TokenType::EqualEqual, "==", 2);
            }
            return makeSingle(TokenType::Equal, c);
        case '!':
            if (getNextChar() == '=') return makeToken(TokenType::NotEqual, "!=", 2);
            return makeSingle(TokenType::Not, c);
        case '<':
            if (getNextChar() == '=') return makeToken(TokenType::LessEqual, "<=", 2);
            return makeSingle(TokenType::Less, c);
        case '>':
            if (getNextChar() == '=') return makeToken(TokenType::GreaterEqual, ">=", 2);
            return makeSingle(TokenType::Greater, c);
        case '-':
            if (getNextChar() == '>') return makeToken(TokenType::Arrow, "->", 2);
            return makeSingle(TokenType::Minus, c);

        // ---- Lookahead sin fallback: si no se extiende, es error lexico ----
        case '&':
            if (getNextChar() == '&') return makeToken(TokenType::AndAnd, "&&" 2);
            break;
        case '|':
            if (getNextChar() == '|') return makeToken(TokenType::OrOr, "||" 2);
            break;
        case '.':
            if (getNextChar() == '.') return makeToken(TokenType::DotDot, "..", 2);
            break;


        // skipWhitespaceComments consume '//' y '/*' antes de llegar aqui, asi que solo /
        case '/': return makeToken(TokenType::Slash, c,1);

        // ---- Literales con estado abierto ----
        case '"':  return readString();
        case '\'': return readChar();

        default:    // Unknown
            break;
    }

    advance();
    return makeToken(TokenType::Unknown, std::string(1, c));
}

void Lexer::skipWhitespaceComments() {
    while(index < source.length()){
        char current = getCurrChar();

        // para espacios en blanco y saltos de linea
        if((current == ' ') || (current == '\t') || (current == '\r') || (current == '\n') ){
            advance();
        } else if ((current == '/' ) && (index + 1 < source.length())){
            char next = source.at(index+1);

            //es comentario de una linea?
            if(next == '/') {
                advance();
                advance();
                while((index < source.length()) && (source.at(index) != '\n')) {
                    advance();
                }
            } else if (next == '*') {
                //comentario multilinea
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
// Avanza 1 char
void Lexer::advance(){
    if(isAtEnd()) return;
    if(source.at(index) == '\n'){
        line++;
        column = 1;
    }else{
        column++;
    }
    index++;
}

char Lexer::getCurrChar() const{
    if (index >= source.size()) return '\0';
    return source.at(index);
}
char Lexer::getNextChar() const{
    if (index + 1 >= source.size()) return '\0';
    return source.at(index + 1);
}
bool Lexer::isAtEnd() const{
    return (index >= source.size());
}

bool Lexer::isDigit(char c){
    return (c >= '0' && c <= '9');
}

bool Lexer::isIdentifier(char c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
// Consume 'skips' caracteres y registra el token en la posicion congelada.
Token Lexer::makeToken(TokenType type, const std::string& lexeme, int skips){
    for(int i = 0 ; i < skips; i++) advance();
    return Token{type, lexeme, tokenLine, tokenColumn};
}


Token Lexer::readNumber(size_t start){
    while(isDigit(getCurrChar())){
        advance();
    }
    if(getCurrChar() == '.' && isDigit(getNextChar())) {
        advance();
        while(isDigit(getCurrChar())){
            advance();
        }
        std::string lexeme = source.substr(start, index - start);
        return makeToken(TokenType::FloatLiteral, lexeme, 0);
    }
    std::string lexeme = source.substr(start, index - start);
    return makeToken(TokenType::IntLiteral, lexeme, 0);

}
Token Lexer::readIdentifier(size_t start){

    while(isIdentifier(getCurrChar())||isDigit(getCurrChar())){
        advance();
    }
    std::string lexeme = source.substr(start, index - start);

    static const std::unordered_map<std::string, TokenType> keywords = {
        {"let",    TokenType::KwLet},
        {"fn",     TokenType::KwFn},
        {"if",     TokenType::KwIf},
        {"else",   TokenType::KwElse},
        {"while",  TokenType::KwWhile},
        {"for",    TokenType::KwFor},
        {"return", TokenType::KwReturn},
        {"in",     TokenType::KwIn},
        {"i32",    TokenType::TypeI32},
        {"f64",    TokenType::TypeF64},
        {"bool",   TokenType::TypeBool},
        {"char",   TokenType::TypeChar},
        {"str",    TokenType::TypeStr},
        {"true",   TokenType::BoolLiteral},
        {"false",  TokenType::BoolLiteral},
    };

    auto T = keywords.find(lexeme);
    if(T != keywords.end()){
        return makeToken(T->second, lexeme);
    }
    return makeToken(TokenType::Identifier, lexeme, 0);
}
Token Lexer::readString(){
    advance(); // comilla de apertura
    std::string value;
    while(!isAtEnd() && getCurrChar() != '"' && getCurrChar() != '\n'){
        if(getCurrChar() == '\\'){
            value += getCurrChar();
            advance();
            if(isAtEnd()) break;
        }
        value += getCurrChar();
        advance();
    }
    if(getCurrChar() != '"')
        return makeToken(TokenType::Unknown, value, 0); // cadena sin cerrar
    advance(); // comilla de cierre
    return makeToken(TokenType::StringLiteral, value, 0);
}

Token Lexer::readChar(){
    advance(); // comilla de apertura
    std::string value;
    if(!isAtEnd() && getCurrChar() == '\\'){
        value += getCurrChar();
        advance();
    }
    if(!isAtEnd() && getCurrChar() != '\'' && getCurrChar() != '\n'){
        value += getCurrChar();
        advance();
    }
    if(getCurrChar() != '\'')
        return makeToken(TokenType::Unknown, value, 0); 
    advance(); // comilla de cierre
    return makeToken(TokenType::CharLiteral, value, 0);
}

