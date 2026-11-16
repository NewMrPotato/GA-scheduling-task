#include <catch2/catch_test_macros.hpp>

#include <genetic_algorithm/ga.hpp>
#include <heterogeneous_machines/generator_population.hpp>
#include <heterogeneous_machines/selections/selection.hpp>
#include <heterogeneous_machines/mutations/mutation.hpp>
#include <heterogeneous_machines/crossovers/crossover.hpp>
#include <heterogeneous_machines/core/communication.hpp>
#include <heterogeneous_machines/core/fitness_counter.hpp>
#include <memory>
#include <optional>
#include <stdexcept>
#include <vector>

using namespace ga::schedule;
using ga::core::Population;

struct ScheduleTestContext {
	TaskGraph graph = {{2}, {2}, {}};
	std::vector<std::size_t> scheduling = {0, 1, 2};
	std::vector<std::size_t> matching = {0, 1, 0};
	std::vector<Machine> machines = {{10.0, 10.0}, {10.0, 10.0}};
	std::vector<std::vector<double>> execution_time = {{2.0, 8.0}, {8.0, 2.0}, {2.0, 2.0}};
	std::vector<double> output_size = {10.0, 10.0, 0.0};

	Population<ScheduleGenome> generate(std::size_t size) {
		PopulationGenerator<ScheduleGenome> generator(
			size, graph, scheduling, matching, machines.size()
		);
		return generator.generate(size, graph);
	}
};

TEST_CASE("Tournament selection keeps evaluated chromosomes", "[schedule][selection]") {
	Population<ScheduleGenome> population;
	population.add({ScheduleGenome({0}, {0}), 2.0});
	TournamentSelection selection;

	auto selected = selection.select(population);
	REQUIRE(selected.size() == 1);
	REQUIRE(selected[0].fitness == 2.0);
	REQUIRE(selected[0].genome.get_matching() == std::vector<std::size_t>{0});

	population[0].fitness.reset();
	REQUIRE_THROWS_AS(selection.select(population), std::invalid_argument);
	REQUIRE(selection.select(Population<ScheduleGenome>{}).empty());
}

TEST_CASE("One point crossover handles an odd population", "[schedule][crossover]") {
	Population<ScheduleGenome> population;
	population.add({ScheduleGenome({0, 0}, {0, 1}), 1.0});
	population.add({ScheduleGenome({1, 1}, {1, 0}), 2.0});
	population.add({ScheduleGenome({0, 1}, {0, 1}), 3.0});
	ScheduleCrossover crossover(1.0);

	crossover.crossover(population);
	REQUIRE(population.size() == 3);
	REQUIRE(population[0].genome.get_matching() == std::vector<std::size_t>{0, 1});
	REQUIRE(population[1].genome.get_matching() == std::vector<std::size_t>{1, 0});
	REQUIRE(population[0].genome.get_scheduling() == std::vector<std::size_t>{0, 1});
	REQUIRE(population[1].genome.get_scheduling() == std::vector<std::size_t>{1, 0});
	REQUIRE_FALSE(population[0].fitness.has_value());
	REQUIRE_FALSE(population[1].fitness.has_value());
	REQUIRE(population[2].fitness == 3.0);
}

TEST_CASE("Mutation keeps valid machine assignments and dependencies", "[schedule][mutation]") {
	ScheduleTestContext context;
	auto population = context.generate(31);
	ScheduleMutation mutation(context.machines.size(), context.graph, 1.0);

	for (auto& chromosome : population) {
		chromosome.fitness = 1.0;
	}
	mutation.mutate(population);

	REQUIRE(population.size() == 31);
	for (const auto& chromosome : population) {
		REQUIRE_FALSE(chromosome.fitness.has_value());
		for (std::size_t machine : chromosome.genome.get_matching()) {
			REQUIRE(machine < context.machines.size());
		}
		auto order = chromosome.genome.get_scheduling();
		REQUIRE(topological_sort(context.graph, order) == order);
	}
}

TEST_CASE("Zero probabilities preserve chromosomes", "[schedule][operators]") {
	ScheduleTestContext context;
	auto population = context.generate(3);
	for (auto& chromosome : population) {
		chromosome.fitness = 1.0;
	}
	auto original = population;
	ScheduleMutation mutation(context.machines.size(), context.graph, 0.0);
	ScheduleCrossover crossover(0.0);

	crossover.crossover(population);
	mutation.mutate(population);
	for (std::size_t i = 0; i < population.size(); ++i) {
		REQUIRE(population[i].genome.get_matching() == original[i].genome.get_matching());
		REQUIRE(population[i].genome.get_scheduling() == original[i].genome.get_scheduling());
		REQUIRE(population[i].fitness == original[i].fitness);
	}
	REQUIRE_THROWS_AS(ScheduleCrossover(-0.1), std::invalid_argument);
	REQUIRE_THROWS_AS(ScheduleMutation(0, context.graph), std::invalid_argument);
	REQUIRE_THROWS_AS(ScheduleMutation(2, context.graph, 1.1), std::invalid_argument);
}

TEST_CASE("Communication uses data size and bottleneck bandwidth", "[schedule][communication]") {
	ScheduleTestContext context;
	ScheduleCommunication<Transfer> communication(context.machines, 20.0, 0.5);

	REQUIRE(communication.time(Transfer{0, 1, 20.0}) == 2.5);
	REQUIRE(communication.time(Transfer{0, 0, 20.0}) == 0.0);
	REQUIRE(communication.time(Transfer{0, 1, 0.0}) == 0.0);
	REQUIRE_THROWS_AS(communication.time(Transfer{0, 2, 10.0}), std::invalid_argument);
	REQUIRE_THROWS_AS(communication.time(Transfer{0, 1, -1.0}), std::invalid_argument);
}

TEST_CASE("Schedule evaluator checks dependencies and parallel execution", "[schedule][fitness]") {
	ScheduleTestContext context;
	ScheduleCommunication<Transfer> communication(context.machines, 10.0, 0.0);
	ScheduleFitnessCounter<ScheduleGenome, Transfer> evaluator(
		communication, context.graph, context.execution_time, context.output_size
	);
	ga::core::Chromosome<ScheduleGenome> chromosome{
		ScheduleGenome({0, 1, 0}, {0, 1, 2}), std::nullopt
	};

	REQUIRE(evaluator.count_fitness(chromosome) == 5.0);
	chromosome.genome = ScheduleGenome({0, 1, 0}, {2, 0, 1});
	REQUIRE_FALSE(evaluator.count_fitness(chromosome).has_value());
	context.graph[0].push_back(0);
	chromosome.genome = ScheduleGenome({0, 1, 0}, {0, 1, 2});
	REQUIRE_FALSE(evaluator.count_fitness(chromosome).has_value());
}

TEST_CASE("Complete scheduling GA runs with all three strategies", "[schedule][ga]") {
	ScheduleTestContext context;
	auto population = context.generate(31);
	ScheduleCommunication<Transfer> communication(context.machines, 10.0, 0.0);
	ScheduleFitnessCounter<ScheduleGenome, Transfer> evaluator(
		communication, context.graph, context.execution_time, context.output_size
	);
	double initial_best = evaluator.count_fitness(population[0]).value();
	for (const auto& chromosome : population) {
		double fitness = evaluator.count_fitness(chromosome).value();
		if (fitness < initial_best) {
			initial_best = fitness;
		}
	}

	ga::core::GeneticAlgorithm<ScheduleGenome> algorithm(
		population,
		std::make_unique<TournamentSelection>(),
		std::make_unique<ScheduleMutation>(context.machines.size(), context.graph),
		std::make_unique<ScheduleCrossover>(),
		std::make_unique<ScheduleFitnessCounter<ScheduleGenome, Transfer>>(
			communication, context.graph, context.execution_time, context.output_size
		),
		std::make_unique<ga::core::StandardStopCriterion<ScheduleGenome>>(50)
	);

	auto best = algorithm.run();
	REQUIRE(algorithm.stats().generation == 50);
	REQUIRE(algorithm.current_population().size() == population.size());
	REQUIRE(best.fitness.has_value());
	REQUIRE(*best.fitness <= initial_best);
	REQUIRE(best.fitness == 5.0);
	REQUIRE(evaluator.count_fitness(best) == best.fitness);

	for (const auto& chromosome : algorithm.current_population()) {
		REQUIRE(evaluator.count_fitness(chromosome) == chromosome.fitness);
	}
}
