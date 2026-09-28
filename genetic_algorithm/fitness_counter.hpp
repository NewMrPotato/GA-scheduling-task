
#pragma once 

#include <population.hpp>
#include <chromosome.hpp>
#include <vector>

template <typename Genome>
struct FitnessCounter{
    //virtual std::vector<double> count(const Population<Genome>& population) = 0;
    virtual double count(const Chromosome<Genome>& chromosome) = 0;

    virtual ~FitnessCounter() = default;
};