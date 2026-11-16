
#pragma once

#include "chromosome.hpp"

namespace ga::core {

template <typename Genome>
struct FitnessCounter{

    virtual std::optional<double> count_fitness(const Chromosome<Genome>& chromosome) = 0;

    virtual ~FitnessCounter() = default;
};

}
