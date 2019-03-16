//
// Created by USER on 15/03/2019.
//

#include "gsymbol.h"

#ifndef CYK_PARSER_GRULE_H
#define CYK_PARSER_GRULE_H


class gRule {
public:
    gSymbol gsymbol;
    symbol_list expansion;
    gRule(gSymbol gsymbol, symbol_list expansion);
    gRule();
    ~gRule();
};

bool operator==(const gRule& lhs, const gRule& rhs);

typedef std::vector<gRule> rule_list;

namespace std {
    template <>
    struct hash<gRule>
    {
        std::size_t operator()(const gRule& k) const;
    };
}

#endif //CYK_PARSER_GRULE_H
