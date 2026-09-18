#ifndef DATA_TYPE_HPP
#define DATA_TYPE_HPP

#include <string>

#include "Token.hpp"

enum class DataType {
    I32, F64, Bool, Char, Str, Unit, Unknown
};

inline std::string toString(DataType dataType) {
    switch (dataType) {
        case DataType::I32:     
            return "i32";
        case DataType::F64:     
            return "f64";
        case DataType::Bool:    
            return "bool";
        case DataType::Char:    
            return "char";
        case DataType::Str:     
            return "str";
        case DataType::Unit:    
            return "()";
        case DataType::Unknown: 
            return "-";
    }
    return "-";
}

inline DataType toDataType(TokenType type) {
    switch (type) {
        case TokenType::TypeI32:  
            return DataType::I32;
        case TokenType::TypeF64:  
            return DataType::F64;
        case TokenType::TypeBool: 
            return DataType::Bool;
        case TokenType::TypeChar: 
            return DataType::Char;
        case TokenType::TypeStr:  
            return DataType::Str;
        default:                  
            return DataType::Unknown;
    }
}

#endif // DATA_TYPE_HPP
