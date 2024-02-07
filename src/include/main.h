#pragma once

#include <iostream>

#include "problem.hpp"
#include "msum.h"
#include "leftlayerone.h"
#include "miscellaneous.h"
#include "toposv2.h"
#include "geneticalgorithm.h"
#include "incrementalnofitsolver.h"

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