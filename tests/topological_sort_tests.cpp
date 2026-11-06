#include "../heterogeneous_machines/core/topological_sort.hpp"

#include <functional>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>

using ga::schedule::TaskGraph;
using ga::schedule::topological_sort;

void require(bool condition, const std::string& message) {
	if (!condition) {
		throw std::runtime_error(message);
	}
}

void test_dependencies() {
	const TaskGraph graph = {{2}, {2}, {3}, {}};
	const auto original = graph;
	const auto order = topological_sort(graph, {0, 1, 2, 3});

	require(order == std::vector<std::size_t>{0, 1, 2, 3}, "Incorrect dependency order");
	require(graph == original, "Sorting changed the graph");
}

void test_preferred_available_task() {
	const TaskGraph graph = {{2}, {3}, {}, {}};
	const std::vector<std::size_t> preference = {3, 1, 2, 0};

	require(topological_sort(graph, preference) == std::vector<std::size_t>{1, 3, 0, 2},
		"Expected the first available task in preference order");
	require(preference == std::vector<std::size_t>{3, 1, 2, 0}, "Sorting changed the preference");
	require(topological_sort(graph, {0, 1, 2, 3}) == std::vector<std::size_t>{0, 1, 2, 3},
		"Changing preference must affect the result");
}

void test_empty_and_independent_tasks() {
	require(topological_sort({}, {}).empty(), "Empty graph must have an empty order");
	require(topological_sort(TaskGraph(3), {2, 0, 1}) == std::vector<std::size_t>{2, 0, 1},
		"Independent tasks were lost");
}

void require_invalid_input(const TaskGraph& graph, const std::vector<std::size_t>& preference) {
	try {
		topological_sort(graph, preference);
	} catch (const std::invalid_argument&) {
		return;
	}

	throw std::runtime_error("Invalid graph was accepted");
}

void require_invalid_graph(const TaskGraph& graph) {
	std::vector<std::size_t> preference(graph.size());
	std::iota(preference.begin(), preference.end(), std::size_t{0});
	require_invalid_input(graph, preference);
}

void test_invalid_preference() {
	require_invalid_input(TaskGraph(3), {0, 1});
	require_invalid_input(TaskGraph(3), {0, 1, 2, 3});
	require_invalid_input(TaskGraph(3), {0, 1, 1});
	require_invalid_input(TaskGraph(3), {0, 1, 3});
}

void test_cycles() {
	require_invalid_graph({{1}, {0}});
	require_invalid_graph({{0}});
	require_invalid_graph({{}, {2}, {1}});
}

void test_invalid_indices() {
	require_invalid_graph({{1}});
}

int run_test(const std::string& name, const std::function<void()>& test) {
	try {
		test();
		std::cout << "[PASS] " << name << '\n';
		return 0;
	} catch (const std::exception& error) {
		std::cerr << "[FAIL] " << name << ": " << error.what() << '\n';
		return 1;
	}
}

int main() {
	int failures = 0;
	failures += run_test("Dependencies", test_dependencies);
	failures += run_test("Preferred available task", test_preferred_available_task);
	failures += run_test("Empty and independent tasks", test_empty_and_independent_tasks);
	failures += run_test("Cycles", test_cycles);
	failures += run_test("Invalid indices", test_invalid_indices);
	failures += run_test("Invalid preference", test_invalid_preference);

	return failures == 0 ? 0 : 1;
}
