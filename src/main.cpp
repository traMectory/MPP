#include "main.h"

#define tms std::chrono::high_resolution_clock::now()
#define dif(a, b) std::chrono::duration_cast<std::chrono::milliseconds>(a - b)

int main(int argc, char** argv)
{
    parseOptions(argc, argv);

    Problem* problem = new Problem(argv[1]);

    problem->roundItems();

    /*Polygon p = Polygon();
    p.push_back(Point(0, 0));
    p.push_back(Point(1, 0));
    p.push_back(Point(1, 1));
    p.push_back(Point(0, 1));

    Polygon c = Polygon();
    c.push_back(Point(0, 0));
    c.push_back(Point(4, 0));
    c.push_back(Point(4, 4));
    c.push_back(Point(0, 4));

    problem = Problem(c, {p, p});*/


    // Problem problem = Problem(argv[1]);

    auto start = tms;

    if (algorithm == 0)
    {
        Msum solver = Msum();
        solver.solve(problem);
    }
    else if (algorithm == 1)
    {
        LeftLayerOne solver = LeftLayerOne();
        solver.solve(problem);
    } else if (algorithm == 2)
    {
        Metaheuristic solver = Metaheuristic();
        solver.solve(problem);
    } 
    else if (algorithm == 3)
    {
        IncrementalNoFitSolver solver = IncrementalNoFitSolver();
        solver.solve(problem);
    }
    else if (algorithm == 4)
    {
        BottomLeft solver = BottomLeft();
        solver.solve(problem);
    }
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
    
    auto time = dif(tms, start);
    problem->addComment("Time: " + std::to_string(time.count()) + "ms");
    problem->addComment("Score: " + std::to_string(problem->getScore()));

    problem->prettyPrint();

    problem->storeSolution(argv[2]);

    if (visualize)
        problem->visualizeSolution();

    // Problem problem2 = Problem(argv[1]);

    // Msum solver2 = Msum();

    // solver2.solve(&problem2);
    // problem2.visualizeSolution();

    // // store comment that was passed through the command line for the output json
    // problem.setComment(comment);

    // if (!solution.empty()) {
    //     std::cout << "loading\n";
    //     problem.loadSolution(solution);
    // }

    // Partition solver = Partition();

    // auto time_1 = std::chrono::high_resolution_clock::now();

    // SolveStatus status = solver.solve(&problem);

    // auto time_2 = std::chrono::high_resolution_clock::now();

    // auto init_time_int = duration_cast<std::chrono::milliseconds>(time_2 - time_1);

    // problem.setTime(init_time_int.count());

    // std::cout << "    took " << init_time_int.count() << "ms\n";

    // if (status == SolveStatus::Feasible || status == SolveStatus::Optimal)
    // {
    //     problem.output();

    //     if (visualize)
    //         problem.visualize();
    // }

    return 0;
}