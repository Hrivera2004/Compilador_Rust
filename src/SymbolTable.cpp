#include "SymbolTable.hpp"

#include <iomanip>

int SymbolTable::insert(const std::string& id) {
    auto found = index_.find(id);
    if (found != index_.end()) {
        return found->second;
    }

    entries_.push_back(Symbol{id, DataType::Unknown});
    int position = static_cast<int>(entries_.size());
    index_[id] = position;
    return position;
}

int SymbolTable::find(const std::string& id) const {
    auto found = index_.find(id);
    if (found == index_.end()) {
        return kInvalidPosition;
    }
    return found->second;
}

bool SymbolTable::setDataType(int position, DataType dataType) {
    if (!isValidPosition(position)) {
        return false;
    }
    entries_[position - 1].dataType = dataType;
    return true;
}

const Symbol* SymbolTable::at(int position) const {
    if (!isValidPosition(position)) {
        return nullptr;
    }
    return &entries_[position - 1];
}

int SymbolTable::size() const {
    return static_cast<int>(entries_.size());
}

void SymbolTable::print(std::ostream& out) const {
    out << std::left << std::setw(5) << "#" << std::setw(16) << "id" << "tipo" << '\n'
        << std::string(28, '-') << '\n';

    for (int position = 1; position <= size(); ++position) {
        const Symbol& symbol = entries_[position - 1];
        out << std::setw(5) << position
            << std::setw(16) << symbol.id
            << toString(symbol.dataType) << '\n';
    }
}

bool SymbolTable::isValidPosition(int position) const {
    return position >= 1 && position <= size();
}
