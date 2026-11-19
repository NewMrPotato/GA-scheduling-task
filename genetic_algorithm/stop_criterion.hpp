
#pragma once

#include "population.hpp"
#include "run_statistics.hpp"
#include "concepts/genome.hpp"


namespace ga::core {

template <GenomeType Genome>
struct StopCriterion{
    virtual bool check(const Population<Genome>& population) = 0;

    virtual bool check(const Population<Genome>& population, const RunStatistics&) {
        return check(population);
    }

    virtual void reset() {}

    virtual ~StopCriterion() = default;
};

}
