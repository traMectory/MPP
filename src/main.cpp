#include "main.h"

#define tms std::chrono::high_resolution_clock::now()
#define dif(a, b) std::chrono::duration_cast<std::chrono::milliseconds>(a - b)

int main(int argc, char **argv)
{
    parseOptions(argc, argv);
    char files[16][150] = { 
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_64ac4991_50.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_x15dff42_50.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_x1869fdd_100.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_x665a736_100.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_6de164e1_200.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_x32f7db7_200.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_54081766_500.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_x6dad719_500.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_x53b606b_1000.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_x51ab828_2000.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_27383a82_5000.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_720892de_5000.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_327d5170_10000.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_x5c10455_10000.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_1943fa88_50000.cgshop2024_instance.json",
        "D:\\uni\\MA\\Packing\\Problems\\cgshop2024_benchmark\\instances\\random_cf1_x6c375be_50000.cgshop2024_instance.json",
    };
    
    for (auto file : files) {
        std::cout << file << "\n";
        Problem* problem = new Problem(file);
        problem->roundItems();


        // Problem problem = Problem(argv[1]);

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
            IncrementalNoFitSolver solver = IncrementalNoFitSolver();
            solver.solve(problem);
        }


        auto time = dif(tms, start);
        problem->addComment("Time: " + std::to_string(time.count()) + "ms");
        problem->addComment("Score: " + std::to_string(problem->getScore()));

        problem->prettyPrint();

        problem->storeSolution(argv[2]);

        if (visualize)
            problem->visualizeSolution();
    }
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