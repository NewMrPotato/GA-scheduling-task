#include "core/topological_sort.hpp"

#include <cstddef>
#include <random>
#include <vector>

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

	return 0;
}
