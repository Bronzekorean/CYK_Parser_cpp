//
// Created by USER on 15/03/2019.
//

#include "gsymbol.h"

gSymbol::gSymbol(std::string symbol, bool is_terminal) {
    this->symbol = symbol;
    this->is_terminal = is_terminal;
}

gSymbol::gSymbol(std::string symbol) {
    if (symbol.find('(') != std::string::npos){
        size_t index = symbol.find('-');
        if (index != std::string::npos){
            symbol = symbol.substr(0, index);
        }
        symbol.erase(std::remove(symbol.begin(), symbol.end(), '('), symbol.end());
        this->symbol = symbol;
        this->is_terminal = false;
    }
    else {
        symbol.erase(std::remove(symbol.begin(), symbol.end(), ')'), symbol.end());
        this->symbol = symbol;
        this->is_terminal = true;
    }
}

gSymbol::gSymbol(){
    this->symbol = std::string();
    this->is_terminal = true;
}

bool operator==(const gSymbol& lhs, const gSymbol& rhs){
    return lhs.is_terminal == rhs.is_terminal && lhs.symbol == rhs.symbol;
}

bool gSymbol::operator!=(gSymbol other) {
    return ! (*this == other);
}

bool operator==(const symbol_list& lhs, const symbol_list& rhs){
    if(lhs.size != rhs.size) return false;
    for(size_t i = 0; i < lhs.size; ++i){
        if (lhs.array[i] != rhs.array[i]) return false;
    }
    return true;
}

namespace std {

    std::size_t hash<gSymbol>::operator()(const gSymbol &k) const {
        using std::size_t;
        using std::hash;
        using std::string;

        // Compute individual hash values for first,
        // second and third and combine them using XOR
        // and bit shifting:

        return (hash<string>()(k.symbol)
                ^ (hash<bool>()(k.is_terminal) << 1));
    }

    std::size_t hash<symbol_list>::operator()(const symbol_list &k) const {
        using std::size_t;
        using std::hash;
        using std::string;

        size_t custom_hash = 0;
        for (size_t i = 0; i < k.size; ++i) {
            custom_hash = (custom_hash
                           ^ (hash<gSymbol>()(k.array[i]) << 1));
        }
        return custom_hash;
    }
}