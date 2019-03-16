//
// Created by USER on 14/03/2019.
//

#include "grammar.h"
#include "grule.h"
#include "gsymbol.h"
#include <algorithm>
#include <ctype.h>
#include <cmath>
#include <sstream>
#include <iterator>
#include <unordered_map>
#include <stack>



rule_logProba::rule_logProba(gRule rule, double log_proba) {
    this->log_proba = log_proba;
    this->rule = rule;
}

rule_list remove_unit_rules(rule_list rules) {

    rule_list out = rule_list();
    std::unordered_map<gSymbol, symbol_set> nt_to_nt = std::unordered_map<gSymbol, symbol_set>();
    for (auto it = rules.begin(); it != rules.end(); ++it) {
        symbol_list expansion = (*it).expansion;
        while (expansion.size == 1 && !expansion.array[0].is_terminal) {
            for (auto it2 = rules.begin(); it2 != rules.end(); ++it2) {
                if ((*it2).gsymbol == expansion.array[0]) {
                    if (nt_to_nt.find((*it2).gsymbol) == nt_to_nt.end()) nt_to_nt[(*it2).gsymbol] = symbol_set();
                    nt_to_nt[(*it2).gsymbol].insert((*it).gsymbol);
                    expansion = (*it2).expansion;
                }
            }
        }
        out.push_back(gRule((*it).gsymbol, expansion));
    }

    rule_list temp;
    for (auto it = out.begin(); it != out.end(); ++it) {
        gSymbol symbol_to_map = (*it).gsymbol;
        if (nt_to_nt.find(symbol_to_map) != nt_to_nt.end()) {
            for (const auto &elem : nt_to_nt[symbol_to_map]) {
                temp.push_back(gRule(elem, (*it).expansion));
            }
        }
    }
    out.insert(out.end(), temp.begin(), temp.end());
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



// TODO: Returned gRule instances go out of scope.
// TODO: gRule destructor shouldn't delete inner array




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



template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

int_list level_list(std::string line) {
    int_list out = int_list();
    int level = 0;
    std::vector<std::string> symbols = split(line, ' ');
    out.size = symbols.size();
    out.array = new int[out.size];
    size_t i = 0;
    for(auto it = symbols.begin(); it != symbols.end(); ++it){
        out.array[i] = level;
        if((*it).find('(') != std::string::npos ) ++level;
        else level -= std::count((*it).begin(), (*it).end(), ')');
        ++i;
    }
    return out;
}

rule_list rules_from_line(std::string line, bool chomsky_normalise){
    std::unordered_map<int, std::vector<int>> tree = std::unordered_map<int, std::vector<int>>();
    int_list levels = level_list(line);
    std::stack<int> lvl_stack = std::stack<int>();
    lvl_stack.push(levels.array[levels.size - 1]);
    for( size_t i = levels.size - 2; i > 0; --i){
        while(! lvl_stack.empty() && lvl_stack.top() == levels.array[i] + 1){
            if(tree.find(i) == tree.end()) tree[i] = std::vector<int>();
            tree[i].push_back(lvl_stack.top());
            lvl_stack.pop();
        }
        lvl_stack.push(i);
    }

    std::vector<std::string> line_split= split(line, ' ');
    symbol_list symbols;
    symbols.size = line_split.size();
    symbols.array = new gSymbol[symbols.size];
    size_t i = 0;
    for(auto it = line_split.begin(); it != line_split.end(); ++it){
        symbols.array[i] = gSymbol(*it);
        ++i;
    }

    rule_list out = rule_list();
    for( auto& it : tree){
        gSymbol root = symbols.array[it.first];
        symbol_list expansion;
        expansion.size = it.second.size();
        expansion.array = new gSymbol[expansion.size];
        i = 0;
        for(auto it1 = it.second.begin(); it1 != it.second.end(); ++it1){
            expansion.array[i] = symbols.array[*it1];
            ++i;
        }
        out.push_back(gRule(root, expansion));
    }

    delete[] symbols.array;

    if(chomsky_normalise){
        out = remove_unit_rules(out);
        rule_list temp;
        for(auto it = out.begin(); it != out.end(); ++it){
            rule_list temp2 = remove_non_solitary_terminals(*it);
            temp.insert(temp.end(), temp2.begin(), temp2.end());
            temp2 = remove_more_than_2NT(*it);
            temp.insert(temp.end(), temp2.begin(), temp2.end());
        }
        out.insert(out.end(), temp.begin(), temp.end());
    }

    return out;
}