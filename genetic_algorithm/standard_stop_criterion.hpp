#pragma once
#include "stop_criterion.hpp"

template <typename Genome>
class StandardStopCriterion : public StopCriterion<Genome> {
public:
    StandardStopCriterion(std::size_t max_generations,
                          std::size_t stagnation_limit = 0,
                          bool stop_on_convergence = false) : 
        max_generations(max_generations), 
        stagnation_limit(stagnation_limit),
        stop_on_convergence(stop_on_convergence) {}

    bool check(const Population<Genome>& population) override {
        if (!stop_on_convergence || population.empty()) return false;

        for (std::size_t i = 0; i < population.size(); ++i) {
            if (!population[i].fitness ||
                population[i].fitness != population[0].fitness) return false;
        }

        return true;
    }

    bool check(const Population<Genome>& population, const RunStatistics& stats) override {
        const bool stop_stat_condition = stats.generation >= max_generations 
        || (stagnation_limit != 0 && stats.generations_without_improvement >= stagnation_limit) 
        || check(population);
        
        return stop_stat_condition;
    }

private:
    std::size_t max_generations;
    std::size_t stagnation_limit;
    bool stop_on_convergence;
};
