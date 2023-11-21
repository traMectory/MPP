#include "geneticalgorithm.h"
#include <random>

SolveStatus GeneticAlgorithm::solve(Problem* prob)
{
	init(prob);
	run(generations);
	auto bestItems = best->getItems();
	for (auto cand: best->getCandidates())
	{
		prob->addCandidate(cand, bestItems[cand.index]->value);
	}
	return SolveStatus::Feasible;
}


void GeneticAlgorithm::init(Problem* prob) {
	std::vector<Item*> flatItems;
	int index = 0;

	//create a flattened version of items vector where each item's quantity is 1
	for (auto item : prob->getItems())
	{
		for (size_t i = 0; i < item->quantity; i++)
		{
			flatItems.push_back(new Item(item->index, item->poly, {}, 1, item->value));
		}
	}

	auto rng = std::default_random_engine{};

	//initialize each individual with a random permutation of items
	for (size_t i = 0; i < populationSize; i++)
	{
		population.push_back(new Problem(prob->getContainer(), {}));
		auto newItems = flatItems;
		std::shuffle(std::begin(newItems), std::end(newItems), rng);
		population[i]->setItems(newItems);
	}

	fitness = std::vector<long long>(populationSize, 0);
	eval();
}


void GeneticAlgorithm::eval() {
	size_t newBest = 0;
	for (size_t i = 0; i < populationSize; i++)
	{
		solver->solve(population[i]);
		fitness[i] = population[i]->getScore();
		if (fitness[i] > fitness[newBest])
			newBest = i;
	}
	best = population[newBest];
	best->visualizeSolution();
}


void GeneticAlgorithm::select() {
	std::vector<Problem*> newPop;
	for (size_t i = 0; i < populationSize; i++) {
		size_t rndInd = std::rand() % populationSize;
		Problem* best = population[rndInd];
		double fit = fitness[rndInd];
		for (int j = 1; j < tournamentK; j++) {
			int contInd = rand() % populationSize;
			Problem* contestant = population[contInd];
			double contFit = fitness[contInd];
			if (contFit > fit) {
				best = contestant;
				fit = contFit;
			}
		}
		newPop.push_back(new Problem(population[i]->getContainer(), {}));
		newPop[i]->setItems(population[i]->getItems());
	}
	population = newPop;
}


void GeneticAlgorithm::mutate(Problem* prob) {
	size_t numMutations = (int)(mutateSwapRate * prob->getNumItems());
	for (size_t i = 0; i < numMutations; i++)
	{
		std::swap(population[rand() % populationSize], population[rand() % populationSize]);
	}
}


void GeneticAlgorithm::run(size_t generations) {
	std::srand(std::time(0));
	for (size_t i = 0; i < generations; i++)
	{
		select();

		for (size_t j = 0; j < populationSize; j++)
		{

			//crossover
			if ((double)std::rand() / RAND_MAX < crossoverRate) {
				//TODO implement ordered crossover
			}

			//mutation
			if ((double)std::rand() / RAND_MAX < mutationRate) {
				mutate(population[j]);
			}
		}
		eval();
		std::cout << "Generation " << i << " done, best score: " << best->getScore() << "std::endl";
	}
}
