#include "main.h"
#include <execution>

#define tms std::chrono::high_resolution_clock::now()
#define dif(a, b) std::chrono::duration_cast<std::chrono::milliseconds>(a - b)

int main(int argc, char **argv)
{
    parseOptions(argc, argv);
    //problem->roundItems();


    Problem* problem = new Problem(argv[1]);

    auto start = tms;

    if (algorithm == 0)
    {
        //Msum solver = Msum();
        //solver.solve(problem);
    }
    else if (algorithm == 1)
    {
        //LeftLayerOne solver = LeftLayerOne();
        //solver.solve(problem);
    }
    else if (algorithm == 3)
    {
        //Toposv2 solver = Toposv2();
        //GeneticAlgorithm ga = GeneticAlgorithm(&solver);
        //ga.solve(problem);
    }
    else if (algorithm == 4)
    {
        //IncrementalNoFitSolver solver = IncrementalNoFitSolver();
        //solver.solve(problem);
        GeneticAlgorithm ga = GeneticAlgorithm<IncrementalNoFitSolver>(GAHyperparams);
        ga.solve(problem);
    }


    auto time = dif(tms, start);
    problem->setRunningTime(time.count());

    problem->prettyPrint();
    problem->storeSolution(argv[2]);

    if (visualize)
        problem->visualizeSolution();

    delete (problem);


    return 0;
}