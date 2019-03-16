//
// Created by USER on 15/03/2019.
//

#include "grule.h"

namespace std {
    std::size_t hash<gRule>::operator()(const gRule &k) const {

        using std::size_t;
        using std::hash;
        using std::string;

        return (hash<gSymbol>()(k.gsymbol)
                ^ (hash<symbol_list>()(k.expansion) << 1));
    }

}

gRule::gRule(gSymbol gsymbol, symbol_list expansion){
    this->gsymbol = gsymbol;
    this->expansion = expansion;
}

bool operator==(const gRule& lhs, const gRule& rhs){
    return lhs.gsymbol == rhs.gsymbol && lhs.expansion == rhs.expansion;
}

gRule::~gRule() {
    delete &(this->gsymbol);
    for(size_t i = 0; i < this->expansion.size; ++i) delete &(this->expansion.array[i]);
    delete &(this->expansion.size);
}
