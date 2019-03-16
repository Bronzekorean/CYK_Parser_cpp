//
// Created by USER on 14/03/2019.
//

#include "grule.h"

#ifndef CYK_PARSER_GRAMMAR_H
#define CYK_PARSER_GRAMMAR_H

struct int_list {
    int* array;
    size_t size;
};

struct rule_logProba {
    gRule rule;
    double log_proba;
    rule_logProba (gRule rule, double log_proba);
};


struct PCFG {
    bool verbose;
    bool to_lower;
    std::unordered_map<gRule, size_t> count_rules;
    std::unordered_map<gSymbol, size_t> normalizer;
    std::unordered_map<gRule, double> log_proabilities;
    std::unordered_map<symbol_list, std::vector<rule_logProba> > reverse_table;
    symbol_set lexicon;
    symbol_set terminals;
    symbol_set non_terminals;
    void update();

    PCFG (std::string* corpus, size_t corpus_length, bool verbose, bool to_lower, bool chomsky_normalize);
};

rule_list remove_unit_rules(rule_list rules);
rule_list remove_non_solitary_terminals(gRule rule);
rule_list remove_more_than_2NT(gRule rule);

int_list level_list(std::string line);



rule_list rules_from_line(std::string line, bool chomsky_normalise);

#endif //CYK_PARSER_GRAMMAR_H
