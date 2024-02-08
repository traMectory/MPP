#ifndef GENETIC_ALGORITHM_COMMON_H
#define GENETIC_ALGORITHM_COMMON_H

#include <vector>
#include <chrono>
#include "problem.hpp"
#include "corner.h"

class Individual {
private:
    std::vector<Item*> permutation;
    long long fitness;
    std::vector<Candidate> candidates;
    std::vector<Strategy> strategies;

public:
    Individual(std::vector<Item*> permutation);
    ~Individual();
    std::vector<Item*> get_permutation();
    void set_fitness(long long fitness);
    long long get_fitness();
    std::vector<Candidate> get_candidates();
    void set_candidates(std::vector<Candidate> candidates);
    std::vector<Strategy> get_strategies();
    void set_strategies(std::vector<Strategy> strategies);
};

#endif