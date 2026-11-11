
#include "core/topological_sort.hpp"
#include "generator_population.hpp"
#include <cstddef>
#include <random>
#include <vector>
#include <iostream>


int main() {
	std::size_t population_size = 5;
	std::size_t machine_count = 3;

	ga::schedule::TaskGraph task_dependencies = {
		{2},
		{3},
		{4},
		{4},
		{}
	};

	std::vector<std::size_t> scheduling = {0, 1, 2, 3, 4};
	std::vector<std::size_t> matching = {0, 1, 2, 0, 1};
	std::mt19937 random_engine{42};

	ga::schedule::PopulationGenerator<ga::schedule::ScheduleGenome> generator(    
		population_size,
		task_dependencies,
		scheduling,
		matching,
		machine_count,
		random_engine
	);
	auto population = generator.generate(
		population_size,
		task_dependencies
	);

	for(auto& chromosome : population){
		std::cout << chromosome;
	}

	return 0;
}
