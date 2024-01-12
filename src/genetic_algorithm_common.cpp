#include "genetic_algorithm_common.h"

Individual::Individual(std::vector<Item*> permutation) {
    this->permutation = permutation;
    this->fitness = 0;
    this->candidates = std::vector<Candidate>();
}

Individual::~Individual()
{
}

std::vector<Item*> Individual::get_permutation() {
    return this->permutation;
}

void Individual::set_fitness(long long fitness) {
    this->fitness = fitness;
}

long long Individual::get_fitness() {
    return this->fitness;
}

std::vector<Candidate> Individual::get_candidates() {
    return this->candidates;
}

void Individual::set_candidates(std::vector<Candidate> candidates) {
    this->candidates = candidates;
}