
#pragma once

#include "population.hpp"

namespace ga::core {

template <typename Genome>
struct Crossover{

    virtual void crossover(Population<Genome>& population) = 0;

    virtual ~Crossover() = default;
};

}
