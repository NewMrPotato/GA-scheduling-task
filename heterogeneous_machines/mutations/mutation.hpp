#pragma once

#include <genetic_algorithm/mutation.hpp>
#include "../core/genome.hpp"
#include "../core/topological_sort.hpp"
#include <cstddef>
#include <random>
#include <stdexcept>
#include <utility>

namespace ga::schedule {

class ScheduleMutation : public ga::core::Mutation<ScheduleGenome> {
private:
	std::size_t machine_count;
	TaskGraph graph;
	double probability;
	std::mt19937 random_engine;

public:
	ScheduleMutation(
		std::size_t machine_count,
		const TaskGraph& graph,
		double probability = 0.2,
		unsigned int seed = 44
	) : machine_count(machine_count), graph(graph),
		probability(probability), random_engine(seed) {
		if (machine_count == 0) {
			throw std::invalid_argument("Mutation requires at least one machine");
		}
		if (!(probability >= 0.0 && probability <= 1.0)) {
			throw std::invalid_argument("Mutation probability must be between 0 and 1");
		}
	}

	void mutate(ga::core::Population<ScheduleGenome>& population) override {
		std::bernoulli_distribution apply(probability);
		std::uniform_int_distribution<std::size_t> choose_machine(0, machine_count - 1);

		for (auto& chromosome : population) {
			if (!apply(random_engine)) {
				continue;
			}

			auto matching = chromosome.genome.get_matching();
			auto scheduling = chromosome.genome.get_scheduling();

			if (matching.size() != graph.size() || scheduling.size() != graph.size()) {
				throw std::invalid_argument("Genome size must match the task graph");
			}
			if (matching.empty()) {
				continue;
			}

			std::uniform_int_distribution<std::size_t> choose_task(0, matching.size() - 1);
			std::size_t task = choose_task(random_engine);
			matching[task] = choose_machine(random_engine);

			std::size_t first = choose_task(random_engine);
			std::size_t second = choose_task(random_engine);
			std::swap(scheduling[first], scheduling[second]);
			scheduling = topological_sort(graph, scheduling);

			chromosome.genome = ScheduleGenome(matching, scheduling);
			chromosome.fitness.reset();
		}
	}
};

}
