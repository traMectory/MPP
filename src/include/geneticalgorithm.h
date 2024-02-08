#pragma once

#include "solver.h"
#include "problem.hpp"
#include <execution>
#include "incrementalnofitsolver.h"

template <class T>
class GeneticAlgorithm : Solver
{
	static_assert(std::is_base_of<Solver, T>::value, "T must be of Type Solver");
public:
    GeneticAlgorithm() {  };

	GeneticAlgorithm(GASpecifics hyperparams) {
		populationSize = hyperparams.populationSize;
		generations = hyperparams.generations;
		mutationRate = hyperparams.mutationRate;
		crossoverRate = hyperparams.crossoverRate;
	};

    size_t populationSize = 100;
    double mutationRate = 0.4;
    double crossoverRate = 0.5;
    size_t generations = 500;
    size_t tournamentK = 2;
    double mutateSwapRate = 0.01;

    SolveStatus solve(Problem* prob);
    void init(Problem* prob);
    void run(size_t generations);

private:
    std::vector<Problem*> population;
    std::vector<long long> fitness;
    void orderCrossover(Problem* p1, Problem* p2);
    void mutate(Problem* prob);
    void eval();
    void select();
    void resetItems();
    Problem* best;
    std::vector<Item*> flatItems;
	size_t cutoffSize;
	std::vector<long long> bestScores;
};



#include <random>

template<class T>
SolveStatus GeneticAlgorithm<T>::solve(Problem* prob)
{
	std::cout << prob->getString() << "\n";

	//run genetic algorithm
	init(prob);
	run(generations);

	//copy best solution to original problem
	bool scoreAdded = false;
	for (auto cand : best->getCandidates())
	{
		prob->addCandidate(cand, scoreAdded? 0 : best->getScore());
		scoreAdded = true;
	}
	prob->setAlgorithmType("genetic");
	prob->setPlacementStrategy(best->getPlacementStrategy());
	GASpecifics specifics({ populationSize,
		generations,
		bestScores,
		mutationRate,
		crossoverRate,
		"single-swap",
		"OX" });
	prob->setAlgorithmSpecifics(specifics);
	for (auto& c : best->getComments()) {
		prob->addComment(c);
	}
	population.clear();
	return SolveStatus::Feasible;
}

template<class T>
void GeneticAlgorithm<T>::init(Problem* prob) {
	int index = 0;

	//create a flattened version of items vector where each item's quantity is 1
	for (auto item : prob->getItems())
	{
		for (size_t i = 0; i < item->quantity; i++)
		{
			flatItems.push_back(new Item(item->index, item->poly, {}, 1, item->value));
		}
	}
	cutoffSize = flatItems.size() / 2;
	auto rng = std::default_random_engine{};

	//sorted from smallest to largest value
	population.push_back(new Problem(prob->getContainer(), {}));
	auto newItems = flatItems;
	std::sort(std::begin(newItems), std::end(newItems), [](const Item* i1, const Item* i2) {
		return (i1->value < i2->value);
		});
	population[0]->setItems(newItems);

	//sorted from largest to smallest value
	population.push_back(new Problem(prob->getContainer(), {}));
	newItems = flatItems;
	std::sort(std::begin(newItems), std::end(newItems), [](const Item* i1, const Item* i2) {
		return (i1->value > i2->value);
		});
	population[1]->setItems(newItems);

	//sorted from smallest to largest area
	population.push_back(new Problem(prob->getContainer(), {}));
	newItems = flatItems;
	std::sort(std::begin(newItems), std::end(newItems), [](const Item* i1, const Item* i2) {
		return i1->poly.area() < i2->poly.area();
		});
	population[2]->setItems(newItems);

	//sorted from largest to smallest area
	population.push_back(new Problem(prob->getContainer(), {}));
	newItems = flatItems;
	std::sort(std::begin(newItems), std::end(newItems), [](const Item* i1, const Item* i2) {
		return i1->poly.area() > i2->poly.area();
		});
	population[3]->setItems(newItems);

	//sorted from largest to smallest value/area
	population.push_back(new Problem(prob->getContainer(), {}));
	newItems = flatItems;
	std::sort(std::begin(newItems), std::end(newItems), [](const Item* i1, const Item* i2) {
		return NT(i1->value)/i1->poly.area() > NT(i2->value)/i2->poly.area();
		});
	population[4]->setItems(newItems);

	//best fit order
	population.push_back(new Problem(prob->getContainer(), {}));
	population[5]->setItems(flatItems);
	IncrementalNoFitSolver::getBestFitOrder(population[4]);


	//initialize each individual with a random permutation of items
	for (size_t i = 6; i < populationSize; i++)
	{
		population.push_back(new Problem(prob->getContainer(), {}));
		auto newItems = flatItems;
		std::shuffle(std::begin(newItems), std::end(newItems), rng);
		population[i]->setItems(newItems);
	}

	fitness = std::vector<long long>(populationSize, 0);
	eval();
}

template<class T>
void GeneticAlgorithm<T>::eval() {
	//evaluate each individual with the score of placed pieces as fitness
	size_t newBest = 0;

	//for (size_t i = 0; i < populationSize; i++)
	std::for_each(std::execution::par, std::begin(population), std::end(population), [&](auto& individual) 
	{
		while (true) {
			Solver* solver = static_cast<Solver*>(new T());
			auto itemsCopy = individual->getItems();
			auto cutoffItems = itemsCopy;
			cutoffItems.resize(cutoffSize);
			individual->setItems(cutoffItems);
			solver->solve(individual);
			individual->setItems(itemsCopy);
			delete solver;
			resetItems();
			if (individual->getNumCandidates() == cutoffSize) {
				cutoffSize = std::min((size_t)(1.1 * cutoffSize), itemsCopy.size());
				individual = new Problem(individual->getContainer(), {});
				individual->setItems(itemsCopy);
				continue;
			}
			break;
		}
	});
	for (size_t i = 0; i < populationSize; i++) {
		fitness[i] = population[i]->getScore();
		if (fitness[i] > fitness[newBest])
			newBest = i;
	}
	best = population[newBest];
	bestScores.push_back(best->getScore());
}

template<class T>
void GeneticAlgorithm<T>::select() {
	std::vector<Problem*> newPop;

	//elitism
	newPop.push_back(new Problem(best->getContainer(), {}));
	newPop[0]->setItems(best->getItems());

	std::srand(std::time(0));

	//tournament selection
	for (size_t i = 1; i < populationSize; i++) {
		size_t rndInd = std::rand() % populationSize;
		Problem* tournamentBest = population[rndInd];
		double fit = fitness[rndInd];
		for (int j = 0; j < tournamentK; j++) {
			int contInd = std::rand() % populationSize;
			Problem* contestant = population[contInd];
			double contFit = fitness[contInd];
			if (contFit > fit) {
				tournamentBest = contestant;
				fit = contFit;
			}
		}
		newPop.push_back(new Problem(tournamentBest->getContainer(), {}));
		newPop[i]->setItems(tournamentBest->getItems());
	}
	for (size_t i = 1; i < populationSize; i++) {
		delete (population[i]);
	}
	population = newPop;
}

template<class T>
void GeneticAlgorithm<T>::mutate(Problem* prob) {
	//mutate genome by randomly swapping a number of polygons
	auto items = prob->getItems();
	size_t numMutations = 1;//std::max((int)(mutateSwapRate * prob->getNumItems()), 1);
	for (size_t i = 0; i < numMutations; i++)
	{
		std::swap(items[rand() % items.size()], items[rand() % items.size()]);
	}
	prob->setItems(items);
}

template<class T>
void GeneticAlgorithm<T>::orderCrossover(Problem* p1, Problem* p2) {
	auto items1 = p1->getItems();
	auto items2 = p2->getItems();
	std::vector<Item*> c1Items(items1.size(), nullptr);
	std::vector<Item*> c2Items(items2.size(), nullptr);

	//items missing in child1 or 2 after copying segments
	std::vector<Item*> missing1;
	std::vector<Item*> missing2;

	size_t crossoverPoints[4];
	crossoverPoints[0] = 0;
	for (size_t i = 1; i < sizeof(crossoverPoints) / sizeof(*crossoverPoints) - 1; i++)
	{
		crossoverPoints[i] = rand() % items1.size();
	}
	crossoverPoints[3] = items1.size() - 1;
	std::sort(std::begin(crossoverPoints), std::end(crossoverPoints));

	//copy items of p1 into c1 and p2 into c2 ich randomly chosen segments
	for (size_t i = 0; i < sizeof(crossoverPoints) / sizeof(*crossoverPoints) - 1; i++)
	{
		for (size_t j = crossoverPoints[i]; j < crossoverPoints[i + 1]; j++)
		{
			if (i % 2 == 0) {
				c1Items[j] = items1[j];
				c2Items[j] = items2[j];
			}
			else
			{
				missing1.push_back(items1[j]);
				missing2.push_back(items2[j]);
			}
		}
	}
	//fill gaps in child1 with missing items in order of p2
	size_t nextInsert = crossoverPoints[1];
	for (auto item : items1)
	{
		if (std::find(missing1.begin(), missing1.end(), item) != missing1.end()) {
			c1Items[nextInsert] = item;
			while (nextInsert < c1Items.size() && c1Items[nextInsert] != nullptr) {
				++nextInsert;
			}
			if (nextInsert >= c1Items.size())
				break;
		}
	}

	//fill gaps in child2 with missing items in order of p1
	nextInsert = crossoverPoints[1];
	for (auto item : items2)
	{
		if (std::find(missing2.begin(), missing2.end(), item) != missing2.end()) {
			c2Items[nextInsert] = item;
			while (nextInsert < c2Items.size() && c2Items[nextInsert] != nullptr) {
				++nextInsert;
			}
			if (nextInsert >= c2Items.size())
				break;
		}
	}

}

template<class T>
void GeneticAlgorithm<T>::resetItems() {
	//reset each item's counter back to 1
	for (auto item : flatItems) {
		item->quantity = 1;
	}
}
template<class T>
void GeneticAlgorithm<T>::run(size_t generations) {
	std::srand(std::time(0));
	std::cout << "Generation 0 done, best score: " << best->getScore() << std::endl;
	for (size_t i = 1; i <= generations-1; i++)
	{
		select();

		for (size_t j = 1; j < populationSize; j++)
		{

			//crossover
			if (2 * j + 1 < populationSize && (double)std::rand() / RAND_MAX < crossoverRate) {
				orderCrossover(population[2 * j], population[2 * j + 1]);
			}

			//mutation
			if ((double)std::rand() / RAND_MAX < mutationRate) {
				mutate(population[j]);
			}
		}
		eval();
		std::cout << "Generation " << i << " done, best score: " << best->getScore() << std::endl;
	}
}
