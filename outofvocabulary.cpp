//
// Created by USER on 16/03/2019.
//

#include "outofvocabulary.h"
#include <algorithm>
#include <set>

int levenshtein_distance(std::string s, std::string t){
    int rows = s.size() + 1;
    int cols = t.size() + 1;
    int** dist = new int*[rows];
    for(int i = 0; i < rows; ++i) dist[i] = new int[cols]();
    for(int i = 1; i < rows; ++i) dist[i][0] = i;
    for(int i = 1; i < cols; ++i) dist[0][i] = i;

    for (int col = 1; col < cols ; ++col) {
        for (int row = 0; row < rows ; ++row) {
            int cost = 0;
            //TODO
            if( s.at(row - 1) != t.at(col - 1) ) cost = 1;
            dist[row][col] = std::min( std::min(dist[row - 1][col] + 1, dist[row][col -1] + 1),
                    dist[row - 1][col - 1]+ cost);
        }
    }
    return dist[rows - 1][cols - 1];
}

oofHandler::oofHandler(PCFG pcfg, int n_word, std::string *words, double **embeddings) {
    this->n_word = n_word;
    this->words = words;
    this->embeddings = embeddings;

    this->terminals_size = pcfg.terminals.size();
    this->terminals = new std::string[this->terminals_size];
    int i = 0;
    for (auto it = pcfg.terminals.begin(); it != pcfg.terminals.end() ; ++it) {
        this->terminals[i] = (*it).symbol;
        ++i;
    }


    std::set<std::string > word_set = std::set<std::string >();
    std::set<std::string > terminal_set = std::set<std::string >();
    for (int i = 0; i < this->n_word; ++i) {
        word_set.insert(this->words[i]);
    }
    for (int i = 0; i < this->terminals_size; ++i) {
        word_set.insert(this->terminals[i]);
    }

    std::vector<std::string > intersection;
    set_intersection(word_set.begin(),word_set.end(),terminal_set.begin(),terminal_set.end(), std::back_inserter(intersection));

    this->embedded_terminals_size = intersection.size();
    this->embedded_terminals = new std::string[this->embedded_terminals_size];
    i  = 0;
    for(auto it = intersection.begin(); it != intersection.end(); ++it){
        this->embedded_terminals[i] = (*it);
        ++i;
    }
}