#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Lexer.hpp"
#include "Parser.hpp"
#include "SymbolTable.hpp"

// Lee el archivo completo; ok indica si se pudo abrir.
std::string readFile(const std::string& fileName, bool& ok){
    std::ifstream file(fileName);

    if (!file.is_open()) {
        std::cerr << fileName << ":0:0: error: no se pudo abrir el archivo\n";
        ok = false;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string file_contents = buffer.str();

    file.close();
    ok = true;
    return file_contents;
}

// Nombre legible de cada TokenType (para imprimir).
const char* tokenTypeName(TokenType type){
    switch (type) {
        case TokenType::Identifier:return "Identifier";
        case TokenType::IntLiteral:return "IntLiteral";
        case TokenType::FloatLiteral:return "FloatLiteral";
        case TokenType::StringLiteral:return "StringLiteral";
        case TokenType::CharLiteral:return "CharLiteral";
        case TokenType::BoolLiteral:return "BoolLiteral";
        case TokenType::KwLet:return "KwLet";
        case TokenType::KwFn:return "KwFn";
        case TokenType::KwIf:return "KwIf";
        case TokenType::KwElse:return "KwElse";
        case TokenType::KwWhile:return "KwWhile";
        case TokenType::KwFor:return "KwFor";
        case TokenType::KwReturn:return "KwReturn";
        case TokenType::KwIn:return "KwIn";
        case TokenType::TypeI32:return "TypeI32";
        case TokenType::TypeF64:return "TypeF64";
        case TokenType::TypeBool:return "TypeBool";
        case TokenType::TypeChar:return "TypeChar";
        case TokenType::TypeStr:return "TypeStr";
        case TokenType::Plus:return "Plus";
        case TokenType::Minus:return "Minus";
        case TokenType::Star:return "Star";
        case TokenType::Slash:return "Slash";
        case TokenType::AndAnd:return "AndAnd";
        case TokenType::OrOr:return "OrOr";
        case TokenType::Not:return "Not";
        case TokenType::EqualEqual:return "EqualEqual";
        case TokenType::NotEqual:return "NotEqual";
        case TokenType::Less:return "Less";
        case TokenType::LessEqual:return "LessEqual";
        case TokenType::Greater:return "Greater";
        case TokenType::GreaterEqual:return "GreaterEqual";
        case TokenType::Equal:return "Equal";
        case TokenType::Arrow:return "Arrow";
        case TokenType::DotDot:return "DotDot";
        case TokenType::LBrace:return "LBrace";
        case TokenType::RBrace:return "RBrace";
        case TokenType::LParen:return "LParen";
        case TokenType::RParen:return "RParen";
        case TokenType::LBracket:return "LBracket";
        case TokenType::RBracket:return "RBracket";
        case TokenType::Comma:return "Comma";
        case TokenType::Semicolon:return "Semicolon";
        case TokenType::Colon:return "Colon";
        case TokenType::EndOfFile:return "EndOfFile";
        case TokenType::Unknown:return "Unknown";
    }
    return "?";
}

// Imprime la tabla TIPO | LEXEMA.
void printTokens(const std::vector<Token>& tokens){
    std::cout << std::left
              << std::setw(15) << "TIPO" << "  "
              << "LEXEMA\n";
    std::cout << std::string(35, '-') << "\n";

    for (const Token& t : tokens) {
        std::cout << std::left
                  << std::setw(15) << tokenTypeName(t.type) << "  "
                  << "'" << t.value << "'\n";
    }
    std::cout << "\n";
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "uso: " << argv[0] << " <archivo.txt>\n";
        return 1;
    }

    std::string fileName = argv[1];
    bool ok = false;
    std::string source = readFile(fileName, ok);
    if (!ok) return 1;

    // ---- Analisis lexico ----
    SymbolTable table;
    Lexer lexer(source, table);
    std::vector<Token> tokens = lexer.tokenize();
    
    

    for (const LexicalError& e : lexer.errors()) {
        std::cerr << fileName << ": error lexico: " << e.message << "\n";
    }

    // ---- Analisis sintactico ----
    Parser parser(tokens, table);
    bool parseOk = parser.parse();

    // ---- Tabla de simbolos (el lexer creo las filas, el parser puso los tipos) ----
    std::cout << "\n --- Tabla de simbolos ---\n\n";
    table.print(std::cout);

    bool lexOk = lexer.errors().empty();

    // ---- Resumen (codigo de salida 0 solo si ambos pasan) ----
    std::cout << "\n==== Resultado ====\n"
              << "Lexer:  " << (lexOk ? "OK" : "con errores (" + std::to_string(lexer.errors().size()) + ")") << "\n"
              << "Parser: " << (parseOk ? "OK" : "con errores") << "\n";

    return (lexOk && parseOk) ? 0 : 1;
}
// g++ -std=c++17 -Wall -Wextra -IResources src/*.cpp -o build/compilador
