
#pragma once

#include <cstddef>

#include <genetic_algorithm/ga.hpp>
#include "core/genome.hpp"
#include "core/population.hpp"
#include "core/topological_sort.hpp"
#include "core/chromosome.hpp"
#include <random>
#include <algorithm>


namespace ga::schedule {

template <typename Genome>
class Generator{

private:
    SchedulePopulation<Genome> population;
    std::mt19937 random_engine;
    std::vector<std::size_t> initial_scheduling;
    std::vector<std::size_t> initial_matching;
    std::size_t maching_count;

public:
    Generator(
        std::size_t population_size,
        TaskGraph task_dependencies,
        std::vector<std::size_t> scheduling,
        std::vector<std::size_t> matching,
        std::size_t machine_count,
        std::mt19937 random_engine = std::mt19937{42}
    )initial_scheduling(scheduling),
     initial_matching(matching), 
     machine_count(machine_count){

        population = SchedulePopulation<Genome>();

        for (int i = 0; i < population_size, i++){
            std::uniform_int_distribution<std::size_t> choose_machine(
                0, machine_count - 1
            );

            std::vector<std::size_t> new_matching(initial_matching.size());

            for (auto& machine : new_matching) {
                machine = choose_machine(random_engine);
            }
            std::shuffle(new_scheduling.begin(), new_scheduling.end(), random_engine);

            std::vector<std::size_t> new_scheduling = initial_scheduling;
            std::vector<std::size_t> sorted_scheduling = topological_sort(task_dependencies, new_scheduling);

            ScheduleGenome genome(new_matching, new_scheduling);
            
            population.add(genome);
        }

        return population;
        
    }
};

}
