//
// Created by USER on 14/03/2019.
//

#include "grammar.h"
#include <algorithm>
#include <ctype.h>
#include <cmath>


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

        std::size_t hash<gSymbol>::operator()(const gSymbol& k) const {
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
            for( size_t i = 0; i < k.size; ++i){
                custom_hash = (custom_hash
                               ^ (hash<gSymbol>()(k.array[i]) << 1));
            }
            return custom_hash;
        }

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

rule_logProba::rule_logProba(gRule rule, double log_proba) {
    this->log_proba = log_proba;
    this->rule = rule;
}

rule_list remove_unit_rules(rule_list rules){

    rule_list out = rule_list();
    return out;
}

rule_list remove_non_solitary_terminals(gRule rule){
    rule_list out = rule_list();
    if(rule.expansion.size < 2) return out;
    for(size_t i = 0; i < rule.expansion.size; ++i){
        gSymbol symbol = rule.expansion.array[i];
        if(symbol.is_terminal){
            gSymbol new_symbol = gSymbol("NT_" + symbol.symbol, false);
            // modify rule
            rule.expansion.array[i] = new_symbol;
            // add new rules to the output
            symbol_list expansion = symbol_list();
            expansion.size = 1;
            expansion.array = new gSymbol[1];
            expansion.array[0] = symbol;
            gRule new_rule = gRule(new_symbol, expansion);
            out.push_back(new_rule);
        }
    }
    return out;
}
rule_list remove_more_than_2NT(gRule rule){
    // we asumme the rule have no solitary terminals
    rule_list out = rule_list();
    if (rule.expansion.size < 2) return out;
    gSymbol last = rule.expansion.array[rule.expansion.size - 1];
    for(size_t i = rule.expansion.size - 2; i != 1; --i){
        gSymbol symbol1 = rule.expansion.array[rule.expansion.size - 2];

        symbol_list expansion = symbol_list();
        expansion.size = 2;
        expansion.array = new gSymbol[2];
        expansion.array[0] = symbol1;
        expansion.array[1] = last;
        last.symbol = symbol1.symbol + "_" + last.symbol;

        gRule new_rule = gRule(last, expansion);
        out.push_back(new_rule);
    }

    for(size_t i = 2; i < rule.expansion.size; ++i) {
        delete (rule.expansion.array + i);
        }
    rule.expansion.size = 2;
    rule.expansion.array[1] = last;

    return out;
}


PCFG::PCFG(std::string *corpus, size_t corpus_length, bool verbose, bool to_lower, bool chomsky_normalize) {
    this->to_lower = to_lower;
    this->count_rules = std::unordered_map<gRule, size_t>();

    for(size_t i = 0; i < corpus_length; ++i){
        std::string line = corpus[i];
        if(this->to_lower) std::transform(line.begin(), line.end(), line.begin(), ::tolower);
        rule_list rules = rules_from_line(line, chomsky_normalize);
        for(auto it = rules.begin(); it != rules.end(); ++it){
            gRule rule = *it;
            if(this->count_rules.find(rule) == this->count_rules.end()) this->count_rules[rule] = 1;
            else this->count_rules[rule] += 1;
        }
    }
    this->update();
}

void PCFG::update() {

    for(auto& it : (this->count_rules)) {
        this->normalizer[it.first.gsymbol] = 0;
    }
    for(auto& it : (this->count_rules)) {
        this->normalizer[it.first.gsymbol] += it.second;
    }

    for(auto& it : (this->count_rules)) {
        this->log_proabilities[it.first] = log((double) it.second / (this->normalizer[it.first.gsymbol]));
    }

    for(auto& it: (this->count_rules)) {
        this->lexicon.insert(it.first.gsymbol);
        if (it.first.gsymbol.is_terminal)
            this->terminals.insert(it.first.gsymbol);
        else
            this->non_terminals.insert(it.first.gsymbol);

        for (size_t i=0; i < it.first.expansion.size; ++i) {
            gSymbol element = it.first.expansion.array[i];
            this->lexicon.insert(element);
            if (element.is_terminal)
                this->terminals.insert(element);
            else
                this->non_terminals.insert(element);
        }
    }
    for(auto& it : this->count_rules) {
        if (this->reverse_table.find(it.first.expansion) == this->reverse_table.end())
        this->reverse_table[it.first.expansion] = std::vector<rule_logProba> ();
        this->reverse_table[it.first.expansion].push_back(rule_logProba(it.first, it.second));

    }
}


