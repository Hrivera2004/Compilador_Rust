#include "Parser.hpp"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens), current(0), hadError(false) {}

// ---- Auxiliares ----

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
    
// Consume y devuelve el token actual; nunca pasa de EndOfFile.
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

// Exige un tipo de token: lo consume o reporta el error y lanza ParseError.
void Parser::expect(TokenType type, const std::string& message){
    if(check(type)){
        advance();
        return;
    }
    error(peek(), message);
    throw ParseError{};
}

// Marca el error y lo imprime (no lanza excepcion).
void Parser::error(const Token& token, const std::string& message){
    hadError = true;

    // El lexer ya reporto los tokens Unknown.
    if(token.type == TokenType::Unknown) return;

    // Evita repetir el mismo error (ej. '}' faltante al final en bloques anidados).
    if(token.line == lastErrorLine && token.column == lastErrorColumn) return;
    lastErrorLine = token.line;
    lastErrorColumn = token.column;

    if(token.type == TokenType::EndOfFile)
        std::cerr << token.line << ":" << token.column
        << ": error: al final del archivo: " << message << "\n";
    else
        std::cerr << token.line << ":" << token.column
        << ": error: en '" << token.value  << "': " << message << "\n";
}

// ---- Recuperacion de errores ----

// Avanza hasta el '}' que cierra el bloque actual, sin consumirlo.
// depth cuenta los bloques internos que se abren mientras se salta.
void Parser::skipToBlockEnd(){
    int depth = 0;
    while(!isAtEnd()){
        if(check(TokenType::LBrace)){
            depth++;
        } else if(check(TokenType::RBrace)){
            if(depth == 0) return;
            depth--;
        }
        advance();
    }
}

// Avanza hasta el siguiente 'fn', sin consumirlo.
void Parser::skipToNextFunction(){
    while(!isAtEnd() && !check(TokenType::KwFn)){
        advance();
    }
}

// ---- Gramatica ----

bool Parser::parse(){
    parseProgram();
    return !hadError;
}

// Programa -> Funcion*
void Parser::parseProgram(){
    while(!isAtEnd()){
        try {
            parseFunction();
        } catch (const ParseError&) {
            skipToNextFunction(); // descarta la funcion con error
        }
    }
}


// Funcion -> 'fn' ID '(' [ID ':' Tipo {',' ID ':' Tipo}] ')' ['->' Tipo] Bloque
void Parser::parseFunction(){
    //validar el 'fn'
    expect(TokenType::KwFn, "Se esperaba 'fn'");

    //validar nombre de la function
    expect(TokenType::Identifier, "Se esperaba el nombre de la funcion");

    //validar (
    expect(TokenType::LParen, "Se esperaba '(' después del nombre de la función");

    //validar parametros
    if(!check(TokenType::RParen)){
        do {
            expect(TokenType::Identifier, "Se esperaba el nombre del parametro");
            expect(TokenType::Colon, "Se esperaba ':' tras nombre de parámetro");
            advance(); // tipo (no se valida)
        } while(match(TokenType::Comma));
    }


    expect(TokenType::RParen, "Se espera ')' al cerrar parámetros");

    if(match(TokenType::Arrow)){
        advance(); // tipo de retorno (no se valida)
    }

    parseBlock(); //siempre entra porque en rust es opcional el tipo de retorno
    
}

// Bloque -> '{' Sentencia* '}'
// Si una sentencia falla, se salta el resto del bloque (como rustc).
void Parser::parseBlock(){
    //que venga si  o si {
    expect(TokenType::LBrace, "Se esperaba '{' al inicio de un bloque");

    while(!check(TokenType::RBrace) && !isAtEnd()){
        try {
            parseStatement();
        } catch (const ParseError&) {
            skipToBlockEnd(); // deja el '}' para el expect de abajo
        }
    }
    expect(TokenType::RBrace, "Se esperaba '}' al final de un bloque");
}

// Sentencia -> Let | If | While | For | Return | Bloque | Expr ';'
void Parser::parseStatement(){
    if(check(TokenType::KwLet)){
        parseLetStatement();
    } else if(check(TokenType::KwIf)){
        parseIfStatement();
    } else if(check(TokenType::KwWhile)){
        parseWhileStatement();
    } else if(check(TokenType::KwFor)){
        parseForStatement();
    } else if (check(TokenType::KwReturn)){
        parseReturnStatement();
    } else if (check(TokenType::LBrace)){
        parseBlock();
    } else {
        parseExpression();
        expect(TokenType::Semicolon, "Se esperaba ';' después de una expresión");
    }

}

// Let -> 'let' ID [':' Tipo] ['=' Expr] ';'
void Parser::parseLetStatement(){
    expect(TokenType::KwLet, "Se esperaba 'let' para declarar una variable");
    expect(TokenType::Identifier, "Se esperaba el nombre de la variable");

    if(match(TokenType::Colon)){
        advance(); // tipo (no se valida)
    }

    if(match(TokenType::Equal)){
        parseExpression();
    }

    expect(TokenType::Semicolon, "Se esperaba ';' al final de la declaración let");
}

// If -> 'if' Expr Bloque ['else' (If | Bloque)]
void Parser::parseIfStatement(){
    expect(TokenType::KwIf, "Se esperaba 'if' al inicio de una declaración if");

    parseExpression();

    parseBlock();

    if(match(TokenType::KwElse)){
        if(check(TokenType::KwIf)){
            parseIfStatement();
        } else {
            parseBlock();
        }
    }
}

// While -> 'while' Expr Bloque
void Parser::parseWhileStatement(){
    expect(TokenType::KwWhile, "Se esperaba 'while' al inicio de una declaración while");

    parseExpression();

    parseBlock();
}

// For -> 'for' ID 'in' Expr Bloque
void Parser::parseForStatement(){
    expect(TokenType::KwFor, "Se esperaba 'for' al inicio de una declaración for");
    expect(TokenType::Identifier, "Se esperaba el nombre de la variable en el ciclo for");
    expect(TokenType::KwIn, "Se esperaba 'in' después del identificador en el ciclo for");

    parseExpression();

    parseBlock();
}

// Return -> 'return' [Expr] ';'
void Parser::parseReturnStatement(){
    expect(TokenType::KwReturn, "Se esperaba 'return' al inicio de la sentencia");
   
    if(!check(TokenType::Semicolon)){
        parseExpression();
    }

    expect(TokenType::Semicolon, "Se esperaba ';' al final de la sentencia return");
}

// Expr     -> ('!' | '-') Expr | Primario {Op Expr}
// Primario -> Literal | ID ['(' [Expr {',' Expr}] ')'] | '(' Expr ')'
// No maneja precedencia: solo valida que la expresion este bien formada.
void Parser::parseExpression(){
    // Unario
    if(match(TokenType::Not) || match(TokenType::Minus)){
        parseExpression();
    // Literal, variable o llamada a funcion
    } else if(match(TokenType::IntLiteral) || match(TokenType::FloatLiteral) || match(TokenType::StringLiteral) || match(TokenType::CharLiteral) || match(TokenType::BoolLiteral) || match(TokenType::Identifier)){
        if(match(TokenType::LParen)){
            if(!check(TokenType::RParen)){
                do{
                    parseExpression();
                } while (match(TokenType::Comma));
            }
            expect(TokenType::RParen, "Se esperaba ')' al cerrar los argumentos de la función");
        }
    // Agrupacion
    } else if(match(TokenType::LParen)) {
        parseExpression();
        expect(TokenType::RParen, "Se esperaba ')' tras expresión entre paréntesis");
    } else {
        error(peek(), "Se esperaba una expresión válida");
        throw ParseError{};
    }

    // Operador binario seguido de otra expresion
    while(check(TokenType::Plus) || check(TokenType::Minus) || check(TokenType::Star) || check(TokenType::Slash) || check(TokenType::EqualEqual) || check(TokenType::NotEqual) || check(TokenType::Less) || check(TokenType::LessEqual) || check(TokenType::Greater) || check(TokenType::GreaterEqual) || check(TokenType::AndAnd) || check(TokenType::OrOr) || check(TokenType::Equal) || check(TokenType::DotDot)){
        advance();
        parseExpression();
    }

}
