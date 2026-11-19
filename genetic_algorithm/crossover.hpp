
#pragma once

#include "population.hpp"
#include "concepts/genome.hpp"

namespace ga::core {

template <GenomeType Genome>
struct Crossover{

    virtual void crossover(Population<Genome>& population) = 0;

    virtual ~Crossover() = default;
};

}
