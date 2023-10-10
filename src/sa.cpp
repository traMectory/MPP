#include "sa.h"

std::vector<Polygon> neighbor(std::vector<Polygon> *S) {
    return {};
}

double cost(std::vector<Polygon> *S) {
    return 0;
}

double randi() {
    return (double) rand() / RAND_MAX;
}

double update() {
    return 0;
}

SolveStatus Sa::solve(Problem *prob)
{

    std::vector<Polygon> S;
    double T = 10000000;
    
    double cS = cost(&S);

    while (false /* stopping criterion */)
    {

        while (false /* not in equilibirium */)
        {

            std::vector<Polygon> Sp = neighbor(&S);

            double cSp = cost(&Sp);
            double delta = cSp - cS;

            double prob = std::min(1.0, std::exp(- delta / T));

            if (randi() <= prob) {
                S = Sp;
                cS = cSp;
            }
        }
        T = update();
    }

    return SolveStatus::Feasible;
}