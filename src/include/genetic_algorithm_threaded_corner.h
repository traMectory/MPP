#ifndef GENETIC_ALGORITHM_THREADED_CORNER_H
#define GENETIC_ALGORITHM_THREADED_CORNER_H

#include <vector>
#include <functional>
#include <random>
#include <thread>
#include <algorithm>
#include <semaphore>
#include <mutex>
#include <queue>
#include "problem.hpp"
#include "solver.h"
#include "bottom_left.h"
#include "genetic_algorithm_common.h"
#include "corner.h"

class GeneticAlgorithmThreadedCorner : public Solver {
private:
    static constexpr int INITIALIZE_INDIVIDUAL = 0;
    static constexpr int QUIT = 1;
    static constexpr int CROSSOVER = 2;
    int state = INITIALIZE_INDIVIDUAL;
    static const int n_threads = 6;
    static const int m = 240;
    std::vector<Individual> population;
    std::function<Individual(long)> fitness;
    Problem *prob{};
    std::vector<std::vector<Candidate>> candidates;
    std::vector<std::vector<Candidate>> generation_candidates;
    std::vector<Individual> generation;
    std::vector<long long> scores;
    std::vector<std::thread> threads;
    std::counting_semaphore<1> *working_threads[6];
    std::mutex mut_population;
    std::mutex mut_generation;
    std::mutex mut_tasks;
    int thread_counter = n_threads;
    std::mutex mut_thread_counter;
    std::counting_semaphore<1000> sem_new_task{0};
    std::counting_semaphore<1000> sem_done{0};
    std::queue<int> tasks;
    std::counting_semaphore<1000> sem_threads{n_threads};
    std::counting_semaphore<1000> sem_tasks{0};
    bool done = false;
    void thread_pool_manager();
    void assign_task();
    void initialization();
    void initialize_individual();
    void proportional_selection(Individual **ind_i, Individual **ind_j);
    void crossover(Individual *i, Individual *j, int index);
    void thread_task(int i);
    void co(int i);

public:
    GeneticAlgorithmThreadedCorner();
    ~GeneticAlgorithmThreadedCorner();
    SolveStatus solve(Problem *prob);
};

#endif
