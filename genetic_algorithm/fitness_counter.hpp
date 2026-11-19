
#pragma once

#include "chromosome.hpp"
#include "concepts/genome.hpp"

namespace ga::core {

template <GenomeType Genome>
struct FitnessCounter{

    virtual std::optional<double> count_fitness(const Chromosome<Genome>& chromosome) = 0;

    virtual ~FitnessCounter() = default;
};

}
