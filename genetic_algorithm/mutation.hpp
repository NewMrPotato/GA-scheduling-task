
#pragma once

#include <population.hpp>

template <typename Genome>
struct Mutation{
    virtual void mutate(Population<Genome>& population) = 0;
};