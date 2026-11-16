#include "core/topological_sort.hpp"
#include "core/machine.hpp"
#include "generator_population.hpp"
#include "core/genome.hpp"
#include "core/chromosome.hpp"
#include "core/communication.hpp"
#include "core/fitness_counter.hpp"
#include <cstddef>
#include <random>
#include <vector>
#include <iostream>
#include <algorithm>
#include <optional>


int main() {
    using namespace ga::schedule;

    std::size_t population_size = 5;
    std::size_t machine_count = 3;

    TaskGraph task_dependencies = {
        {2},
        {3},
        {4},
        {4},
        {}
    };

    std::vector<std::size_t> scheduling = {0, 1, 2, 3, 4};
    std::vector<std::size_t> matching = {0, 1, 2, 0, 1};

    std::vector<Machine> machines = {
        {100.0, 50.0},
        {200.0, 80.0},
        {150.0, 120.0}
    };

    double network_bandwidth = 100.0;
    double network_latency = 0.01;

    std::vector<std::vector<double>> execution_time = {
        {5.0, 8.0, 3.0},
        {7.0, 4.0, 6.0},
        {2.0, 9.0, 5.0},
        {8.0, 3.0, 7.0},
        {6.0, 4.0, 2.0}
    };

    std::vector<double> output_size = {
        100.0,
        60.0,
        80.0,
        40.0,
        0.0
    };

    std::mt19937 random_engine{42};

    PopulationGenerator<ScheduleGenome> generator(
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

    ScheduleCommunication<Transfer> communication(
        machines,
        network_bandwidth,
        network_latency
    );

    ScheduleFitnessCounter<ScheduleGenome, Transfer> fitness_counter(
        communication,
        task_dependencies,
        execution_time,
        output_size
    );

    ScheduleChromosome<ScheduleGenome> test_chromosome{
        {ScheduleGenome(matching, scheduling), std::nullopt}
    };

    std::cout << "Reference chromosome:\n";
    std::cout << test_chromosome;
    std::cout << "Calculated fitness: "
              << *fitness_counter.count_fitness(test_chromosome)
              << "\n\n";

    for (auto& chromosome : population) {
        chromosome.fitness = fitness_counter.count_fitness(chromosome);
        std::cout << chromosome << '\n';
    }

}