
#pragma once

#include <population.hpp>

template <typename Genome>
struct StopCriterion{
    virtual bool check(const Population<Genome>& population) = 0;

    virtual ~StopCriterion() = default;
};