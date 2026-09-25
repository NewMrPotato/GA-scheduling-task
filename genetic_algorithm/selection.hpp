
#pragma once

#include <population.hpp>

template <typename Genome>
struct Selection{
    virtual Population<Genome> select(const Population<Genome>& population) = 0;
};