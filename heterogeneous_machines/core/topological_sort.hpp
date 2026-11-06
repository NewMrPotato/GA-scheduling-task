#pragma once

#include <cstddef>
#include <functional>
#include <queue>
#include <stdexcept>
#include <vector>

namespace ga::schedule {

using TaskGraph = std::vector<std::vector<std::size_t>>;

inline std::vector<std::size_t> topological_sort(
	const TaskGraph& graph,
	const std::vector<std::size_t>& task_order
) {
	if (task_order.size() != graph.size()) {
		throw std::invalid_argument("Task order must contain every task exactly once");
	}

	std::vector<std::size_t> priority(graph.size(), graph.size());
	for (std::size_t position = 0; position < task_order.size(); ++position) {
		const std::size_t task = task_order[position];
		if (task >= graph.size() || priority[task] != graph.size()) {
			throw std::invalid_argument("Task order contains an invalid or duplicate task");
		}
		priority[task] = position;
	}

	std::vector<std::size_t> indegree(graph.size(), 0);

	for (const auto& successors : graph) {
		for (std::size_t task : successors) {
			if (task >= graph.size()) {
				throw std::invalid_argument("Task dependency index is out of range");
			}
			++indegree[task];
		}
	}

	std::priority_queue<std::size_t, std::vector<std::size_t>, std::greater<std::size_t>> ready;

	for (std::size_t task = 0; task < graph.size(); ++task) {
		if (indegree[task] == 0) {
			ready.push(priority[task]);
		}
	}

	std::vector<std::size_t> order;
	order.reserve(graph.size());

	while (!ready.empty()) {
		const std::size_t task = task_order[ready.top()];
		ready.pop();
		order.push_back(task);

		for (std::size_t successor : graph[task]) {
			if (--indegree[successor] == 0) {
				ready.push(priority[successor]);
			}
		}
	}

	if (order.size() != graph.size()) {
		throw std::invalid_argument("Task dependencies contain a cycle");
	}

	return order;
}

}
