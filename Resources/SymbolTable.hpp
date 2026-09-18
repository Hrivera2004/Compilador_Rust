#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "DataType.hpp"

struct Symbol {
    std::string id;
    DataType dataType;
};

class SymbolTable {
public:
    static constexpr int kInvalidPosition = 0;
    int insert(const std::string& id);
    int find(const std::string& id) const;
    bool setDataType(int position, DataType dataType);
    const Symbol* at(int position) const;
    void print(std::ostream& out) const;

private:
    int size() const;
    bool isValidPosition(int position) const;

    std::vector<Symbol> entries_;
    std::unordered_map<std::string, int> index_;
};

#endif // SYMBOL_TABLE_HPP
