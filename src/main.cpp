#include "main.h"
#include <execution>

#define tms std::chrono::high_resolution_clock::now()
#define dif(a, b) std::chrono::duration_cast<std::chrono::milliseconds>(a - b)

int main(int argc, char **argv)
{
    parseOptions(argc, argv);
    //problem->roundItems();


    Problem* problem = new Problem(argv[1]);

    problem->roundItems();

    /*Polygon p = Polygon();
    p.push_back(Point(0, 0));
    p.push_back(Point(1, 0));
    p.push_back(Point(1, 1));
    p.push_back(Point(0, 1));

    Polygon c = Polygon();
    c.push_back(Point(0, 0));
    c.push_back(Point(50, 0));
    c.push_back(Point(50, 5));
    c.push_back(Point(0, 5));

    std::vector<Polygon> polygons;

    for (int i = 0; i < 50; i++) {
        polygons.push_back(p);
    }
    problem = new Problem(c, polygons);*/


    //Problem problem = Problem(argv[1]);

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
    // else if (algorithm == 4)
    // {
    //     BottomLeft solver = BottomLeft();
    //     solver.solve(problem);
    // }
    else if (algorithm == 5)
    {
        GeneticAlgorithmSimple solver = GeneticAlgorithmSimple();
        solver.solve(problem);
    } 
    else if (algorithm == 6)
    {
        GeneticAlgorithmThreaded solver = GeneticAlgorithmThreaded();
        solver.solve(problem);
    }
    else if (algorithm == 7)
    {
        GeneticAlgorithmThreadedIncremental solver = GeneticAlgorithmThreadedIncremental();
        solver.solve(problem);
    }
    else if (algorithm == 8)
    {
        GeneticAlgorithmThreadedCorner solver = GeneticAlgorithmThreadedCorner();
        solver.solve(problem);
    }
    else if (algorithm == 9)
    {
        std::vector<Strategy> strategies;
        for (int i = 0; i < problem->getItems().size(); i++) {
            strategies.push_back(TR);
        }
        Corner solver = Corner(strategies);
        solver.solve(problem);
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