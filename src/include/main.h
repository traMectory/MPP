#pragma once

#include <iostream>

#include "problem.hpp"
#include "msum.h"
#include "leftlayerone.h"
#include "miscellaneous.h"
#include "incrementalnofitsolver.h"
#include "metaheuristic1.h"
#include "bottom_left.h"
#include "genetic_algorithm_simple.h"
#include "genetic_algorithm_threaded.h"
#include "genetic_algorithm_threaded_incremental.h"
#include "genetic_algorithm_threaded_corner.h"

bool visualize = false;
int algorithm = 4;

GASpecifics GAHyperparams({
    10, //populationSize;
    10, //generations;
    {}, //leave blank
    0.2, //mutationRate;
    0.6, //crossoverRate;
    "SingleSwap", //mutationType;
    "OX" //crossoverType;
});