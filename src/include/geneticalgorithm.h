#pragma once

#include "solver.h"
#include "problem.hpp"

class GeneticAlgorithm : Solver
{
public:
    GeneticAlgorithm(Solver* s) { solver = s; };

    size_t populationSize = 30;
    double mutationRate = 0.5;
    double crossoverRate = 0.6;
    size_t generations = 20;
    size_t tournamentK = 3;
    double mutateSwapRate = 0.2;

    SolveStatus solve(Problem* prob);
    void init(Problem* prob);
    void run(size_t generations);

private:
    Solver* solver;
    std::vector<Problem*> population;
    std::vector<long long> fitness;
    void orderCrossover(Problem* p1, Problem* p2);
    void mutate(Problem* prob);
    void eval();
    void select();
    void resetItems();
    Problem* best;
    std::vector<Item*> flatItems;
};