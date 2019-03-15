//
// Created by USER on 14/03/2019.
//
#include <xstring>
#include <unordered_map>
#include <set>
#include <vector>

#ifndef CYK_PARSER_GRAMMAR_H
#define CYK_PARSER_GRAMMAR_H

struct int_list {
    int* array;
    size_t size;
};

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

rule_list remove_unit_rules(rule_list rules);
rule_list remove_non_solitary_terminals(gRule rule);
rule_list remove_more_than_2NT(gRule rule);

namespace std {

    template <>
    struct hash<gSymbol>
    {
        std::size_t operator()(const gSymbol& k) const;
    };

    template <>
    struct hash<symbol_list>
    {
        std::size_t operator()(const symbol_list& k) const;
    };

    template <>
    struct hash<gRule>
    {
        std::size_t operator()(const gRule& k) const;
    };
}

struct rule_logProba {
    gRule rule;
    double log_proba;
    rule_logProba (gRule rule, double log_proba);
};


class PCFG {
    bool verbose;
    bool to_lower;
    std::unordered_map<gRule, size_t> count_rules;
    std::unordered_map<gSymbol, size_t> normalizer;
    std::unordered_map<gRule, double> log_proabilities;
    std::unordered_map<symbol_list, std::vector<rule_logProba> > reverse_table;
    std::set<gSymbol> lexicon;
    std::set<gSymbol> terminals;
    std::set<gSymbol> non_terminals;
    void update();

    PCFG (std::string* corpus, size_t corpus_length, bool verbose, bool to_lower, bool chomsky_normalize);
    ~PCFG();
};


int_list level_list(std::string line);



rule_list rules_from_line(std::string line, bool chomsky_normalise);

#endif //CYK_PARSER_GRAMMAR_H
