#pragma once

#include "solver.h"
#include "problem.hpp"

class GeneticAlgorithm : Solver
{
public:
    GeneticAlgorithm(Solver* s) { solver = s; };

    size_t populationSize = 10;
    double mutationRate = 0.1;
    double crossoverRate = 0.5;
    size_t generations = 10;
    size_t tournamentK = 2;
    double mutateSwapRate = 0.2;

    SolveStatus solve(Problem* prob);
    void init(Problem* prob);
    void run(size_t generations);

private:
    Solver* solver;
    std::vector<Problem*> population;
    std::vector<long long> fitness;
    Problem* orderedCrossover(Problem* parent1, Problem* parent2);
    void mutate(Problem* prob);
    void eval();
    void select();
    Problem* best;
};