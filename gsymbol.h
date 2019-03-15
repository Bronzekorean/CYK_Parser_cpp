//
// Created by USER on 15/03/2019.
//

#include <xstring>
#include <unordered_map>
#include <set>


#ifndef CYK_PARSER_GSYMBOL_H
#define CYK_PARSER_GSYMBOL_H

class gSymbol {
public:
    std::string symbol;
    bool is_terminal;
    bool operator!=(gSymbol other);
    gSymbol(std::string symbol, bool is_terminal);
    gSymbol(std::string symbol);
    gSymbol();
};

bool operator==(const gSymbol& lhs, const gSymbol& rhs);

struct symbol_list {
    gSymbol* array;
    size_t size;
};

bool operator==(const symbol_list& lhs, const symbol_list& rhs);


namespace std {

    template<>
    struct hash<gSymbol> {
        std::size_t operator()(const gSymbol &k) const;
    };

    template<>
    struct hash<symbol_list> {
        std::size_t operator()(const symbol_list &k) const;
    };
}


#endif //CYK_PARSER_GSYMBOL_H