//
// Created by USER on 16/03/2019.
//
#include "grammar.h"

#ifndef CYK_PARSER_CYKPARSER_H
#define CYK_PARSER_CYKPARSER_H

struct cyk_table_value{
    gRule rule;
    double logProba;
    int length_first;
    int start_first;
    int length_second;
    int size_second;
    cyk_table_value(gRule rule, double logProba, int length_first, int start_first, int length_second, int size_second);
    ~cyk_table_value();
};

typedef std::unordered_map<gSymbol, cyk_table_value>** cyk_table;

struct cykParser {
    PCFG pcfg;
    gSymbol root_symbol;

    cykParser(PCFG pcfg, gSymbol root_symbol, bool handleoov, std::string embedding_path);
    inline cykParser(PCFG pcfg, gSymbol root_symbol){
        cykParser(pcfg, root_symbol, false, std::string(""));
    }
    std::string bracketed_from_cyk(cyk_table table, size_t length, size_t k, gSymbol symbol);
    std::string parse(std::string line);
    std::string* parse(std::string* sentences, int n_sentences);
};


#endif //CYK_PARSER_CYKPARSER_H
