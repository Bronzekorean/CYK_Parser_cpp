//
// Created by USER on 16/03/2019.
//
#include "grammar.h"

#ifndef CYK_PARSER_OUTOFVOCABULARY_H
#define CYK_PARSER_OUTOFVOCABULARY_H

int levenshtein_distance(std::string s, std::string t);
struct oofHandler {
    int n_word;
    std::string* words;
    double** embeddings;
    static const int embedding_dimension = 64;

    int terminals_size;
    std::string* terminals;

    int embedded_terminals_size;
    std::string* embedded_terminals;

    oofHandler(PCFG pcfg, int n_word, std::string* words, double** embeddings);
    std::string closer_levenshtein(std::string word);
    std::string case_normalizer(std::string word);
    std::string normalize(std::string word);
    std::string nearest_cosine(std::string word);
    std::string replace(std::string word);
};


#endif //CYK_PARSER_OUTOFVOCABULARY_H
