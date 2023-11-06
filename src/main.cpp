#include "main.h"

int main(int argc, char **argv)
{
    parseOptions(argc, argv);

    Problem problem = Problem(argv[1]);


    // Problem problem = Problem(argv[1]);

    if (algorithm == 0)
    {
        Msum solver = Msum();
        solver.solve(&problem);
    }
    else if (algorithm == 1)
    {
        LeftLayer solver = LeftLayer();
        solver.solve(&problem);
    }
    else if (algorithm == 2)
    {
        LeftLayerOne solver = LeftLayerOne();
        solver.solve(&problem);
    }
    else if (algorithm == 3)
    {
        Toposv2 solver = Toposv2();
        solver.solve(&problem);
    }

    problem.prettyPrint();

    problem.storeSolution();

    if (visualize)
        problem.visualizeSolution();

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