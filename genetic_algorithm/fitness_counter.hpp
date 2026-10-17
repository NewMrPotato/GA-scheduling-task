
#pragma once

#include "chromosome.hpp"

template <typename Genome>
struct FitnessCounter{

    virtual double count(const Chromosome<Genome>& chromosome) = 0;

    virtual ~FitnessCounter() = default;
};
