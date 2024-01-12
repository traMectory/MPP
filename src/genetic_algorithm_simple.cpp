#include "genetic_algorithm_common.h"
#include "genetic_algorithm_simple.h"

GeneticAlgorithmSimple::GeneticAlgorithmSimple() {
    this->population = std::vector<Individual>();
    this->generation = std::vector<Individual>();
    this->candidates = std::vector<std::vector<Candidate>>(this->m);
    this->generation_candidates = std::vector<std::vector<Candidate>>();
    this->scores = std::vector<long long>(this->m);
}

GeneticAlgorithmSimple::~GeneticAlgorithmSimple() {
}


void GeneticAlgorithmSimple::co(int i) {
    Individual *ind_i, *ind_j = nullptr;
    this->proportional_selection(&ind_i, &ind_j);
    this->crossover(ind_i, ind_j, i);
}

void GeneticAlgorithmSimple::initialize_individual() {
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

SolveStatus GeneticAlgorithmSimple::solve(Problem *prob) {
    // Thread initialization

    this->prob = prob;

    // Initialization (Calculating fitness for initial individuals)
    this->initialization();
    std::cout << "Best fitness after initialization: " << this->population[this->population.size() - 1].get_fitness() << std::endl;

    // Generate g new generation
    for (int g = 0; g < 100; g++) {
        // Send crossover tasks to thread pool
        for (int i = 0; i < m; i++) {
            this->co(i);
        }

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
        /*for (int i = 0; i < this->population[this->population.size() - 1].get_candidates().size(); i++) {
            prob->addCandidate(this->population[this->population.size() - 1].get_candidates()[i], this->population[this->population.size() - 1].get_permutation()[i]->value);
        }
        //std::stringstream s;
        //s << "temp_" << g << ".out";
        //prob->storeSolution(s.str());*/
        prob->clearCandidates();

        std::cout << "["  << g << "]: " << this->population[this->population.size() - 1].get_fitness() << std::endl;
        this->generation.clear();
    }

    for (int i = 0; i < this->population[this->population.size() - 1].get_candidates().size(); i++) {
        prob->addCandidate(this->population[this->population.size() - 1].get_candidates()[i], this->population[this->population.size() - 1].get_permutation()[i]->value);
    }
    std::cout << "Quitting!" << std::endl;
    return SolveStatus::Feasible;
}

void GeneticAlgorithmSimple::initialization() {
    for (int i = 0; i < this->m; i++) {
        this->initialize_individual();
    }

    std::sort(this->population.begin(), this->population.end(),
        [](Individual x, Individual y) {
             return x.get_fitness() < y.get_fitness();
        });    
}

void GeneticAlgorithmSimple::proportional_selection(Individual **ind_i, Individual **ind_j) {
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

void GeneticAlgorithmSimple::crossover(Individual *ind_i, Individual *ind_j, int index) {
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
