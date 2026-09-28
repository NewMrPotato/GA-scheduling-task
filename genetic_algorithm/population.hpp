
#pragma once

#include <vector>
#include <chromosome.hpp>

template <typename Genome>
struct Population{
    std::vector<Chromosome<Genome>> chromosomes;
};