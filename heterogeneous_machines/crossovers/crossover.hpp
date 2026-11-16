#pragma once

#include <genetic_algorithm/crossover.hpp>
#include "../core/genome.hpp"
#include <cstddef>
#include <random>
#include <stdexcept>
#include <utility>

namespace ga::schedule {

class ScheduleCrossover : public ga::core::Crossover<ScheduleGenome> {
private:
	double probability;
	std::mt19937 random_engine;

public:
	ScheduleCrossover(double probability = 0.8, unsigned int seed = 43)
		: probability(probability), random_engine(seed) {
		if (!(probability >= 0.0 && probability <= 1.0)) {
			throw std::invalid_argument("Crossover probability must be between 0 and 1");
		}
	}

	void crossover(ga::core::Population<ScheduleGenome>& population) override {
		std::bernoulli_distribution apply(probability);

		for (std::size_t i = 0; i + 1 < population.size(); i += 2) {
			if (!apply(random_engine)) {
				continue;
			}

			auto first_matching = population[i].genome.get_matching();
			auto second_matching = population[i + 1].genome.get_matching();

			if (first_matching.size() != second_matching.size()) {
				throw std::invalid_argument("Parents must have the same number of tasks");
			}
			if (first_matching.size() < 2) {
				continue;
			}

			std::uniform_int_distribution<std::size_t> choose_cut(1, first_matching.size() - 1);
			std::size_t cut = choose_cut(random_engine);

			for (std::size_t task = cut; task < first_matching.size(); ++task) {
				std::swap(first_matching[task], second_matching[task]);
			}

			population[i].genome = ScheduleGenome(
				first_matching, population[i].genome.get_scheduling()
			);
			population[i + 1].genome = ScheduleGenome(
				second_matching, population[i + 1].genome.get_scheduling()
			);
			population[i].fitness.reset();
			population[i + 1].fitness.reset();
		}
	}
};

}
