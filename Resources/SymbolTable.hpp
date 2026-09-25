#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <ostream>
#include <string>
#include <vector>

#include "DataType.hpp"

struct Symbol {
    std::string id;
    DataType dataType;
};

class SymbolTable {
public:
    //siempre agrega una fila nueva (se permiten nombres repetidos).
    int insert(const std::string& id, DataType dataType);
    const Symbol* at(int position) const;
    void print(std::ostream& out) const;

private:
    int size() const;
    bool isValidPosition(int position) const;

    std::vector<Symbol> entries_;
};

#endif
