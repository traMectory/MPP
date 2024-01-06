#include "genetic_algorithm.h"

GeneticAlgorithm::GeneticAlgorithm() {
    this->population = std::vector<Individual>();
    this->generation = std::vector<Individual>();
    this->candidates = std::vector<std::vector<Candidate>>(this->m);
    this->generation_candidates = std::vector<std::vector<Candidate>>();
    this->scores = std::vector<long long>(this->m);
    this->threads = std::vector<std::thread>();
    for (int i = 0; i < this->n_threads; i++) {
        std::counting_semaphore<1> *s = new std::counting_semaphore<1>(1); 
        this->working_threads[i] = s;
    }
    this->tasks = std::queue<int>();
}

GeneticAlgorithm::~GeneticAlgorithm() {
}

void GeneticAlgorithm::thread_task(int i) {
    while (!done) {
        this->sem_tasks.acquire();
        this->mut_thread_counter.lock();
        this->thread_counter--;
        this->mut_tasks.lock();
        int task = this->tasks.front();
        this->tasks.pop();
        bool empty = tasks.empty();
        this->mut_tasks.unlock();
        this->mut_thread_counter.unlock();


        switch (task) {
        case INITIALIZE_INDIVIDUAL:
            this->initialize_individual();
            break;
        case QUIT:
            this->mut_thread_counter.lock();
            this->sem_threads.release();
            this->thread_counter++;
            if (this->tasks.empty()) {
                this->sem_done.release();
                done = true;
                this->sem_new_task.release();
            }
            this->mut_thread_counter.unlock();
            return;
        case CROSSOVER:
            this->co(i);
            break;
        default:
            break;
        }
        this->mut_thread_counter.lock();
        this->sem_threads.release();
        this->thread_counter++;
        if (thread_counter == this->n_threads && this->tasks.empty()) {
            sem_done.release();
        }
        this->mut_thread_counter.unlock();
    }
}

void GeneticAlgorithm::co(int i) {
    Individual *ind_i, *ind_j = nullptr;
    this->proportional_selection(&ind_i, &ind_j);
    this->crossover(ind_i, ind_j, i);
}

void GeneticAlgorithm::initialize_individual() {
    std::random_device rd;
    std::mt19937 g(rd());
    std::vector<Item*> items;

    Polygon container = prob->getContainer();
    items = prob->getItems();
    std::shuffle(items.begin(), items.end(), g);
    Individual individual = Individual(items);

    std::vector<Candidate> c;
    long long score = 0;
    bl(container, individual.get_permutation(), &c, &score);
    individual.set_fitness(score);
    individual.set_candidates(c);
    this->mut_population.lock();
    this->population.push_back(individual);
    this->mut_population.unlock();
}

void GeneticAlgorithm::thread_pool_manager() {
    while (!done) {
        this->sem_new_task.acquire();
        this->sem_threads.acquire();
        this->sem_tasks.release();
    }
}

SolveStatus GeneticAlgorithm::solve(Problem *prob) {
    // Thread initialization
    threads.push_back(std::thread(&GeneticAlgorithm::thread_pool_manager, std::ref(*this)));
    for (int i = 0; i < this->n_threads; i++) {
        this->threads.push_back(std::thread(&GeneticAlgorithm::thread_task, std::ref(*this), i));
    }    

    this->prob = prob;

    // Initialization (Calculating fitness for initial individuals)
    this->initialization();
    std::cout << "Best fitness after initialization: " << this->population[this->population.size() - 1].get_fitness() << std::endl;

    // Generate g new generation
    for (int g = 0; g < 100; g++) {
        // Send crossover tasks to thread pool
        for (int i = 0; i < m; i++) {
            this->tasks.push(CROSSOVER);
            sem_new_task.release();
        }

        // Wait for all crossover tasks to be done
        this->sem_done.acquire();

        // Update population
        for (int i = 0; i < this->generation.size(); i++) {
            if (this->generation[i].get_fitness() > this->population[0].get_fitness()) {
                Individual descendant = this->generation[i];

                int i = 0;
                for (; i < population.size(); i++) {
                    if (this->population[i].get_fitness() >= descendant.get_fitness()) {
                        break;
                    }
                }
                this->population.insert(population.begin() + i, descendant);
                this->population.erase(this->population.begin());
            }
        }
        for (int i = 0; i < this->population[this->population.size() - 1].get_candidates().size(); i++) {
            prob->addCandidate(this->population[this->population.size() - 1].get_candidates()[i], this->population[this->population.size() - 1].get_permutation()[i]->value);
        }
        std::stringstream s;
        s << "temp_" << g << ".out";
        prob->storeSolution(s.str());
        prob->getCandidates().clear();

        std::cout << "["  << g << "]: " << this->population[this->population.size() - 1].get_fitness() << std::endl;
        this->generation.clear();
    }

    // Send tasks to thread pool for quitting
    for (int i = 0; i < this->n_threads; i++) {
        tasks.push(QUIT);
        sem_new_task.release();
    }

    sem_done.acquire();

    for (int i = 0; i < this->n_threads + 1; i++) {
        this->threads[i].join();
    }

    for (int i = 0; i < this->population[this->population.size() - 1].get_candidates().size(); i++) {
        prob->addCandidate(this->population[this->population.size() - 1].get_candidates()[i], this->population[this->population.size() - 1].get_permutation()[i]->value);
    }
    std::cout << "Quitting!" << std::endl;
    return SolveStatus::Feasible;
}

void GeneticAlgorithm::initialization() {
    for (int i = 0; i < this->m; i++) {
        this->tasks.push(INITIALIZE_INDIVIDUAL);
        this->sem_new_task.release();
    }

    this->sem_done.acquire();

    std::sort(this->population.begin(), this->population.end(),
        [](Individual x, Individual y) {
             return x.get_fitness() < y.get_fitness();
        });    
}

void GeneticAlgorithm::proportional_selection(Individual **ind_i, Individual **ind_j) {
    std::vector<double> probabilities;

    long long sum = 
        std::accumulate(this->population.begin(), this->population.end(), 0LL,
            [](long long acc, Individual i) {
                return acc + i.get_fitness();
            });
    
    for (Individual i : this->population) {
        double p = ((double) i.get_fitness()) / sum;
        probabilities.push_back(p);
    }

    for (int i = 0; i < probabilities.size() - 1; i++) {
        probabilities[i + 1] += probabilities[i];
    }
    
    std::uniform_real_distribution<double> dist(0, 1);
    std::mt19937 rng;
    rng.seed(std::random_device{}());

    double i = dist(rng);
    double j = dist(rng);

    *ind_i = &this->population[0];
    *ind_j = &this->population[0];

    for (int p = 0; p < probabilities.size() - 1; p++) {
        if (i > probabilities[p] && i <= probabilities[p + 1]) {
            *ind_i = &this->population[p + 1];
        }
        if (j > probabilities[p] && j <= probabilities[p + 1]) {
            *ind_j = &this->population[p + 1];
        }
    } 
}

void GeneticAlgorithm::crossover(Individual *ind_i, Individual *ind_j, int index) {
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, ind_i->get_permutation().size());
    int p = dist(rng);
    int q = dist(rng);

    std::vector<Item*> perm_desc;

    for (int i = p; i < ind_i->get_permutation().size() && i < p + q; i++) {
        perm_desc.push_back(ind_i->get_permutation()[i]);
    }

    for (int i = 0; i < ind_j->get_permutation().size(); i++) {
        if (i >= p && i < p + q) {
            continue;
        }
        if (std::find(perm_desc.begin(), perm_desc.end(), ind_j->get_permutation()[i]) == perm_desc.end()) {
            perm_desc.push_back(ind_j->get_permutation()[i]);
        }
    }

    Individual individual = Individual(perm_desc);

    std::vector<Candidate> c;
    long long score = 0;

    bl(this->prob->getContainer(), individual.get_permutation(), &c, &score);

    individual.set_fitness(score);
    individual.set_candidates(c);
    this->mut_generation.lock();
    this->generation.push_back(individual);
    this->mut_generation.unlock();
}

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