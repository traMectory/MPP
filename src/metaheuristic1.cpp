#include "metaheuristic1.h"

SolveStatus Metaheuristic::solve(Problem *prob)
{

    long long bestScore = 0;

    auto rd = std::random_device{}; // comment if you want every run to be the same
    auto rng = std::default_random_engine{rd()};

    for (int i = 0; i < 10; i++)
    {
        // deep copy and randomize order of items
        std::vector<Item *> items;
        for (Item* item : prob->getItems())
        {
            items.push_back(new Item{item->index, item->poly, item->quantity, item->value});
        }
        std::shuffle(std::begin(items), std::end(items), rng);

        // solve using this order
        Problem intermediate = Problem(prob->getContainer(), items, prob->getNumItems());
        LeftLayerOne blackbox = LeftLayerOne();
        blackbox.solve(&intermediate);


        if (intermediate.getScore() > bestScore)
        {
            bestScore = intermediate.getScore();
            prob->clearCandidates();
            for (Candidate c : intermediate.getCandidates())
            {
                prob->addCandidate(c, 0);
                prob->setScore(bestScore);
            }
            std::cout << bestScore << std::endl;
        }

        // resolve pointers
        for (Item* item : items)
        {
            delete item;
        }
        items.clear();
    }

    return SolveStatus::Feasible;
}