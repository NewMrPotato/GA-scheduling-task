
#pragma once

#include "population.hpp"
#include "concepts/genome.hpp"


namespace ga::core {

template <GenomeType Genome>
struct Selection{

    virtual Population<Genome> select(const Population<Genome>& population) = 0;

    virtual ~Selection() = default;
};

}
