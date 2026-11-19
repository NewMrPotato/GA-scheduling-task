
#pragma once

#include "population.hpp"
#include "concepts/genome.hpp"

namespace ga::core {

template <GenomeType Genome>
struct Mutation{

    virtual void mutate(Population<Genome>& population) = 0;

    virtual ~Mutation() = default;
};

}
