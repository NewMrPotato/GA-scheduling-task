#pragma once

#include <genetic_algorithm/selection.hpp>
#include "../core/genome.hpp"
#include <cmath>
#include <cstddef>
#include <random>
#include <stdexcept>

namespace ga::schedule {

class TournamentSelection : public ga::core::Selection<ScheduleGenome> {
private:
	std::mt19937 random_engine;

public:
	TournamentSelection(unsigned int seed = 42) : random_engine(seed) {}

	ga::core::Population<ScheduleGenome> select(
		const ga::core::Population<ScheduleGenome>& population
	) override {
		ga::core::Population<ScheduleGenome> selected;
		if (population.empty()) {
			return selected;
		}

		for (const auto& chromosome : population) {
			if (!chromosome.fitness || !std::isfinite(*chromosome.fitness)) {
				throw std::invalid_argument("Selection requires finite fitness values");
			}
		}

		std::uniform_int_distribution<std::size_t> choose(0, population.size() - 1);

		for (std::size_t i = 0; i < population.size(); ++i) {
			std::size_t first = choose(random_engine);
			std::size_t second = choose(random_engine);

			if (*population[first].fitness <= *population[second].fitness) {
				selected.add(population[first]);
			} else {
				selected.add(population[second]);
			}
		}

		return selected;
	}
};

}
