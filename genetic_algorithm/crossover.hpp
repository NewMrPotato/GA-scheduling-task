
#pragma once

#include <population.hpp>

template <typename Genome>
struct Crossover{
    virtual void crossover(Population<Genome>& population) = 0;

    virtual ~Crossover() = default;
};