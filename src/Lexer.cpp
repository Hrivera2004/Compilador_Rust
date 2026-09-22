#include "Lexer.hpp"
#include <unordered_map>

Lexer::Lexer(const std::string& source)
    : source(source) {}

// Pide tokens hasta llegar a EndOfFile (incluido).
std::vector<Token> Lexer::tokenize(){
    std::vector<Token> tokens;
    while(true){
        Token t = nextToken();
        tokens.push_back(t);
        if(t.type == TokenType::EndOfFile) break;
    }
    return tokens;
}

// Decide el tipo de token segun el primer caracter.
Token Lexer::nextToken(){
    skipWhitespaceComments();

    tokenLine = line;
    tokenColumn = column;

    if(isAtEnd())
        return makeToken(TokenType::EndOfFile, "");

    char c = getCurrChar();

    if (isDigit(c))
        return readNumber();

    if (isIdentifier(c))
        return readIdentifier();

    switch(c){
        // ---- Delimitadores (un caracter, sin lookahead) ----
        case '{': return makeToken(TokenType::LBrace, std::string(1, c), 1);
        case '}': return makeToken(TokenType::RBrace, std::string(1, c), 1);
        case '(': return makeToken(TokenType::LParen, std::string(1, c), 1);
        case ')': return makeToken(TokenType::RParen, std::string(1, c), 1);
        case '[': return makeToken(TokenType::LBracket, std::string(1, c), 1);
        case ']': return makeToken(TokenType::RBracket, std::string(1, c), 1);
        case ',': return makeToken(TokenType::Comma, std::string(1, c), 1);
        case ';': return makeToken(TokenType::Semicolon, std::string(1, c), 1);
        case ':': return makeToken(TokenType::Colon, std::string(1, c), 1);

        // ---- Operadores simples ----
        case '+': return makeToken(TokenType::Plus, std::string(1, c), 1);
        case '*': return makeToken(TokenType::Star, std::string(1, c), 1);

        // ---- Lookahead con fallback valido ----
        case '=':
            if (getNextChar() == '='){
                    return makeToken(TokenType::EqualEqual, "==", 2);
            }
            return makeToken(TokenType::Equal, std::string(1, c), 1);
        case '!':
            if (getNextChar() == '=') return makeToken(TokenType::NotEqual, "!=", 2);
            return makeToken(TokenType::Not, std::string(1, c), 1);
        case '<':
            if (getNextChar() == '=') return makeToken(TokenType::LessEqual, "<=", 2);
            return makeToken(TokenType::Less, std::string(1, c), 1);
        case '>':
            if (getNextChar() == '=') return makeToken(TokenType::GreaterEqual, ">=", 2);
            return makeToken(TokenType::Greater, std::string(1, c), 1);
        case '-':
            if (getNextChar() == '>') return makeToken(TokenType::Arrow, "->", 2);
            return makeToken(TokenType::Minus, std::string(1, c), 1);

        // ---- Lookahead sin fallback: si no se extiende, es error lexico ----
        case '&':
            if (getNextChar() == '&') return makeToken(TokenType::AndAnd, "&&", 2);
            advance();
            return makeUnknown("se esperaba '&&', se encontro '&'", std::string(1, c));
        case '|':
            if (getNextChar() == '|') return makeToken(TokenType::OrOr, "||", 2);
            advance();
            return makeUnknown("se esperaba '||', se encontro '|'", std::string(1, c));
        case '.':
            if (getNextChar() == '.') return makeToken(TokenType::DotDot, "..", 2);
            advance();
            return makeUnknown("se esperaba '..', se encontro '.'", std::string(1, c));


        // skipWhitespaceComments consume '//' y '/*' antes de llegar aqui, asi que solo /
        case '/': return makeToken(TokenType::Slash, std::string(1, c), 1);

        // ---- Literales con estado abierto ----
        case '"':  return readString();
        case '\'': return readChar();

        default:    // Unknown
            break;
    }

    // Caracter que no pertenece al lenguaje
    advance();
    return makeUnknown("caracter inesperado '" + std::string(1, c) + "'", std::string(1, c));
}

// Salta espacios y comentarios hasta el siguiente caracter util.
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
                int startLine = line;
                int startColumn = column;
                bool closed = false;
                advance();
                advance();
                while(index < source.length()){
                    if ((source.at(index) == '*') && (index + 1 < source.length()) && (source.at(index + 1) == '/')){
                        advance();
                        advance();
                        closed = true;
                        break;
                    }
                    advance();
                }
                if(!closed){
                    errors_.push_back(LexicalError{"comentario de bloque sin terminar", startLine, startColumn});
                    hadError = true;
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
// Avanza 1 char (no pasa del final)
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

// Lookahead: devuelven '\0' si se sale del fuente.
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

// Caracter que puede iniciar un identificador (los digitos se aceptan despues).
bool Lexer::isIdentifier(char c){
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

// Escapes reconocidos por el lenguaje.
bool Lexer::isValidEscape(char c){
    return c == 'n' || c == 't' || c == 'r' || c == '0'
        || c == '\\' || c == '\'' || c == '"';
}
// Consume 'skips' caracteres y registra el token en la posicion congelada.
Token Lexer::makeToken(TokenType type, const std::string& lexeme, int skips){
    for(int i = 0 ; i < skips; i++) advance();
    return Token{type, lexeme, tokenLine, tokenColumn};
}
// Registra el error y el unknown en la posicion congelada.
Token Lexer::makeUnknown(const std::string& message, const std::string& lexeme){
    errors_.push_back(LexicalError{ message, tokenLine, tokenColumn});
    hadError = true;
    return Token{TokenType::Unknown, lexeme, tokenLine, tokenColumn};
}

// Lee un entero (42) o un float (3.75, 5.).
Token Lexer::readNumber(){
    TokenType type = TokenType::IntLiteral;
    std::string lexeme;
    while(isDigit(getCurrChar())){
        lexeme += getCurrChar();
        advance();
    }
    // Parte decimal: '.' que no sea rango (0..10) ni acceso (5.a)
    if(getCurrChar() == '.' && !isIdentifier(getNextChar()) && (getNextChar() != '.')) {
        type = TokenType::FloatLiteral;
        lexeme += getCurrChar();
        advance();
        while(isDigit(getCurrChar())){
            lexeme += getCurrChar();
            advance();
        }
        // Segundo punto decimal (1.5.3): numero mal formado
        if(getCurrChar() == '.' && isDigit(getNextChar())) {
            while(isDigit(getCurrChar()) || isIdentifier(getCurrChar())
            || (getCurrChar() == '.' && isDigit(getNextChar()))
            ){
                lexeme += getCurrChar();
                advance();
            }
            return makeUnknown("numero mal formado", lexeme);
        }
    }
    // Letras pegadas al numero (12abc, 1.5x)
    if(isIdentifier(getCurrChar())){
        while(isIdentifier(getCurrChar()) || isDigit(getCurrChar())){
            lexeme += getCurrChar();
            advance();
        }
        if(type == TokenType::FloatLiteral)
            return makeUnknown("numero mal formado", lexeme);

        return makeUnknown("identificador no puede empezar con digito", lexeme);
    }
    return makeToken(type, lexeme, 0);
}
// Lee un identificador y revisa si es palabra reservada, tipo o bool.
Token Lexer::readIdentifier(){
    std::string lexeme;
    while(isIdentifier(getCurrChar())||isDigit(getCurrChar())){
        lexeme += getCurrChar();
        advance();
    }

    // Palabras con token propio
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
// Lee "..." (puede ocupar varias lineas). El valor no incluye las comillas.
Token Lexer::readString(){
    advance(); // comilla de apertura
    std::string value;
    bool valid = true; // false si hay un escape invalido

    while(!isAtEnd() && getCurrChar() != '"'){
        if(getCurrChar() == '\\'){
            value += '\\';
            advance();
            if(isAtEnd()) break;
            if(!isValidEscape(getCurrChar()))
            valid = false;
            value += getCurrChar();
            advance();
            continue; // el caracter escapado no se revisa contra el cierre
        }
        value += getCurrChar();
        advance();
    }

    if(isAtEnd())
        return makeUnknown("cadena sin terminar", value);

    advance(); // comilla de cierre
    if(!valid)
        return makeUnknown("secuencia de escape invalida", value);
    return makeToken(TokenType::StringLiteral, value, 0);
}

// Lee 'c' o un escape como '\n'. Debe tener exactamente un caracter.
Token Lexer::readChar(){
    advance(); // comilla de apertura
    std::string value;

    // Sin contenido
    if(isAtEnd() || getCurrChar() == '\n')
        return makeUnknown("literal de caracter sin terminar", value);

    // ''
    if(getCurrChar() == '\''){
        advance();
        return makeUnknown("literal de caracter vacio", value);
    }
    bool valid = true;
    if(getCurrChar() == '\\'){
        value += '\\';
        advance();
        if(isAtEnd() || getCurrChar() == '\n')
            return makeUnknown("literal de caracter sin terminar", value);
        if(!isValidEscape(getCurrChar()))
            valid = false;
        value += getCurrChar();
        advance();
    } else {
        value += getCurrChar();
        advance();
    }

    // Mas de un caracter: se consume hasta el cierre para no corromper lo que sigue.
    if(getCurrChar() != '\''){
        while(!isAtEnd() && getCurrChar() != '\'' && getCurrChar() != '\n'){
            value += getCurrChar();
            advance();
        }
        if(getCurrChar() != '\'')
            return makeUnknown("literal de caracter sin terminar", value);
        advance();
        return makeUnknown("literal de caracter con mas de un caracter", value);
    }

    advance(); // comilla de cierre
    if(!valid)
        return makeUnknown("secuencia de escape invalida", value);
    return makeToken(TokenType::CharLiteral, value, 0);
}
