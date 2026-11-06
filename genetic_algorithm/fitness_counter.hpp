
#pragma once

#include "chromosome.hpp"

namespace ga::core {

template <typename Genome>
struct FitnessCounter{

    virtual double count(const Chromosome<Genome>& chromosome) = 0;

    virtual ~FitnessCounter() = default;
};

}
