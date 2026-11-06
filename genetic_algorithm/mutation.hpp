
#pragma once

#include "population.hpp"

namespace ga::core {

template <typename Genome>
struct Mutation{

    virtual void mutate(Population<Genome>& population) = 0;

    virtual ~Mutation() = default;
};

}
