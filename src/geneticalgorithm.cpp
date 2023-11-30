#include "geneticalgorithm.h"
#include <random>

SolveStatus GeneticAlgorithm::solve(Problem* prob)
{
	//run genetic algorithm
	init(prob);
	run(generations);

	//copy best solution to original problem
	auto bestItems = best->getItems();
	for (auto cand: best->getCandidates())
	{
		prob->addCandidate(cand, bestItems[cand.index]->value);
	}
	return SolveStatus::Feasible;
}


void GeneticAlgorithm::init(Problem* prob) {
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
	//evaluate each individual with the score of placed pieces as fitness
	size_t newBest = 0;
	for (size_t i = 0; i < populationSize; i++)
	{
		solver->solve(population[i]);
		resetItems();
		fitness[i] = population[i]->getScore();
		if (fitness[i] > fitness[newBest])
			newBest = i;
	}
	best = population[newBest];
}


void GeneticAlgorithm::select() {
	std::vector<Problem*> newPop;

	//elitism
	newPop.push_back(new Problem(best->getContainer(), {}));
	newPop[0]->setItems(best->getItems());

	//tournament selection
	for (size_t i = 1; i < populationSize; i++) {
		size_t rndInd = std::rand() % populationSize;
		Problem* tournamentBest = population[rndInd];
		double fit = fitness[rndInd];
		for (int j = 1; j < tournamentK; j++) {
			int contInd = rand() % populationSize;
			Problem* contestant = population[contInd];
			double contFit = fitness[contInd];
			if (contFit > fit) {
				tournamentBest = contestant;
				fit = contFit;
			}
		}
		newPop.push_back(new Problem(population[i]->getContainer(), {}));
		newPop[i]->setItems(population[i]->getItems());
	}
	population = newPop;
}


void GeneticAlgorithm::mutate(Problem* prob) {
	//mutate genome by randomly swapping a number of polygons
	auto items = prob->getItems();
	size_t numMutations = (int)(mutateSwapRate * prob->getNumItems());
	for (size_t i = 0; i < numMutations; i++)
	{
		std::swap(items[rand() % items.size()], items[rand() % items.size()]);
	}
	prob->setItems(items);
}


void GeneticAlgorithm::orderCrossover(Problem* p1, Problem* p2) {
	auto items1 = p1->getItems();
	auto items2 = p2->getItems();
	std::vector<Item*> c1Items(items1.size(), nullptr);
	std::vector<Item*> c2Items(items2.size(), nullptr);

	//items missing in child1 or 2 after copying segments
	std::vector<Item*> missing1;
	std::vector<Item*> missing2;

	size_t crossoverPoints[4];
	crossoverPoints[0] = 0;
	for (size_t i = 1; i < sizeof(crossoverPoints)/sizeof(*crossoverPoints) - 1; i++)
	{
		crossoverPoints[i] = rand() % items1.size();
	}
	crossoverPoints[3] = items1.size() - 1;
	std::sort(std::begin(crossoverPoints), std::end(crossoverPoints));
	
	//copy items of p1 into c1 and p2 into c2 ich randomly chosen segments
	for (size_t i = 0; i < sizeof(crossoverPoints) / sizeof(*crossoverPoints) - 1; i++)
	{
		for (size_t j = crossoverPoints[i]; j < crossoverPoints[i+1]; j++)
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
	for (auto item: items1)
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


void GeneticAlgorithm::resetItems() {
	//reset each item's counter back to 1
	for (auto item : flatItems) {
		item->quantity = 1;
	}
}

void GeneticAlgorithm::run(size_t generations) {
	std::srand(std::time(0));
	std::cout << "Generation " << 0 << " done, best score: " << best->getScore() << std::endl;
	for (size_t i = 1; i <= generations; i++)
	{
		select();

		for (size_t j = 1; j < populationSize; j++)
		{

			//crossover
			if (2*j+1 < populationSize && (double)std::rand() / RAND_MAX < crossoverRate) {
				orderCrossover(population[2*j], population[2*j+1]);
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
