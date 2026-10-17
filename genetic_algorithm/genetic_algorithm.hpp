
#pragma once

#include "population.hpp"
#include "selection.hpp"
#include "mutation.hpp"
#include "crossover.hpp"
#include "fitness_counter.hpp"
#include "stop_criterion.hpp"
#include "run_statistics.hpp"
#include <cmath>
#include <functional>
#include <optional>
#include <memory>
#include <stdexcept>
#include <utility>

template <typename Genome>
class GeneticAlgorithm {
public:

    GeneticAlgorithm(Population<Genome> initial_population,
                     std::unique_ptr<Selection<Genome>> selection,
                     std::unique_ptr<Mutation<Genome>> mutation,
                     std::unique_ptr<Crossover<Genome>> crossover,
                     std::unique_ptr<FitnessCounter<Genome>> fitness_counter,
                     std::unique_ptr<StopCriterion<Genome>> stop_criterion,
                     std::function<bool(double, double)> better = std::less<double>{}) 
        : population(std::move(initial_population)),
        selection(std::move(selection)),
        mutation(std::move(mutation)),
        crossover(std::move(crossover)),
        fitness_counter(std::move(fitness_counter)),
        stop_criterion(std::move(stop_criterion)),
        better(std::move(better)) {

        if (population.empty()) {
            throw std::invalid_argument("Population must not be empty");
        }
        if (!this->selection) {
            throw std::invalid_argument("Selection must not be null");
        }
        if (!this->mutation) {
            throw std::invalid_argument("Mutation must not be null");
        }
        if (!this->crossover) {
            throw std::invalid_argument("Crossover must not be null");
        }
        if (!this->fitness_counter) {
            throw std::invalid_argument("Fitness counter must not be null");
        }
        if (!this->stop_criterion) {
            throw std::invalid_argument("Stop criterion must not be null");
        }
        if (!this->better) {
            throw std::invalid_argument("Fitness func must not be null");
        }
    }

    Chromosome<Genome> run() {
        if (!selection || !mutation || !crossover || !fitness_counter || !stop_criterion || !better)
            throw std::logic_error("GA dependencies are not initialized");
        if (population.empty()) throw std::invalid_argument("Empty population");

        statistics = {};
        elite.reset();

        stop_criterion->reset();
        evaluate();

        elite = population[best_index()];
        statistics.best_fitness = elite->fitness.value();
        
        while (!stop_criterion->check(population, statistics)) {
            const auto expected_size = population.size();
            auto next = selection->select(population);
            require_size(next, expected_size);
            replace_population(next);
            invalidate_fitness();

            crossover->crossover(population);
            require_size(population, expected_size);
            invalidate_fitness();

            mutation->mutate(population);
            require_size(population, expected_size);
            evaluate();

            const auto best = best_index();
            ++statistics.generation;

            if (better(population[best].fitness.value(), elite->fitness.value())) {
                elite = population[best];
                statistics.generations_without_improvement = 0;
            } else {
                ++statistics.generations_without_improvement;
                if (better(elite->fitness.value(), population[best].fitness.value()))
                    population[worst_index()] = elite.value();
            }
            statistics.best_fitness = elite->fitness.value();
        }
        return elite.value();
    }

    const RunStatistics& stats() const { return statistics; }
    const Population<Genome>& current_population() const { return population; }

private:
    void replace_population(Population<Genome>& next) {

        population = std::move(next);
    }

    static void require_size(const Population<Genome>& value, std::size_t expected) {
        if (value.size() != expected)
            throw std::logic_error("GA operators must preserve population size");
    }

    void invalidate_fitness() {
        for (std::size_t i = 0; i < population.size(); ++i) 
            population[i].fitness.reset();
    }

    void evaluate() {
        invalidate_fitness();

        for (std::size_t i = 0; i < population.size(); ++i) {
            const double score = fitness_counter->count(population[i]);
            if (!std::isfinite(score)) throw std::domain_error("Non-finite fitness");
            population[i].fitness = score;
        }
    }

    std::size_t best_index() const {
        std::size_t best = 0;

        for (std::size_t i = 1; i < population.size(); ++i)
            if (better(population[i].fitness.value(), population[best].fitness.value())) best = i;
        return best;
    }

    std::size_t worst_index() const {
        std::size_t worst = 0;

        for (std::size_t i = 1; i < population.size(); ++i)
            if (better(population[worst].fitness.value(), population[i].fitness.value())) worst = i;
        return worst;
    }

    Population<Genome> population;
    std::unique_ptr<Selection<Genome>> selection;
    std::unique_ptr<Mutation<Genome>> mutation;
    std::unique_ptr<Crossover<Genome>> crossover;
    std::unique_ptr<FitnessCounter<Genome>> fitness_counter;
    std::unique_ptr<StopCriterion<Genome>> stop_criterion;
    std::function<bool(double, double)> better;
    std::optional<Chromosome<Genome>> elite;
    RunStatistics statistics;

};
