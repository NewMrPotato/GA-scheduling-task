
#pragma once

#include "population.hpp"

namespace ga::core {

template <typename Genome>
struct Selection{

    virtual Population<Genome> select(const Population<Genome>& population) = 0;

    virtual ~Selection() = default;
};

}
