#pragma once

#include <genetic_algorithm/ga.hpp>
#include "genome.hpp"
#include "communication.hpp"
#include "chromosome.hpp"
#include "topological_sort.hpp"
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <optional>
#include <vector>


namespace ga::schedule {

template <typename Genome, typename Transfer>
class ScheduleFitnessCounter
    : public ga::core::CommunicatingFitnessCounter<Genome, Transfer> {

public:
    ScheduleFitnessCounter(
        const ScheduleCommunication<Transfer>& model,
        const TaskGraph& graph,
        const std::vector<std::vector<double>>& execution_time,
        const std::vector<double>& output_size
    )
        : ga::core::CommunicatingFitnessCounter<Genome, Transfer>(model),
          graph_(graph),
          execution_time_(execution_time),
          output_size_(output_size) {}

    std::optional<double> count_fitness(
        const ga::core::Chromosome<Genome>& chromosome
    ) override {
        const auto matching = chromosome.genome.get_matching();
        const auto scheduling = chromosome.genome.get_scheduling();

        std::size_t task_count = graph_.size();

        if (scheduling.size() != task_count ||
            matching.size() != task_count ||
            execution_time_.size() != task_count ||
            output_size_.size() != task_count)
            return std::nullopt;

        if (task_count == 0)
            return 0.0;

        std::size_t machine_count = execution_time_[0].size();

        if (machine_count == 0)
            return std::nullopt;

        for (const auto& row : execution_time_) {
            if (row.size() != machine_count)
                return std::nullopt;
        }

        std::vector<double> machine_free(machine_count, 0.0);
        std::vector<double> data_ready(task_count, 0.0);
        std::vector<bool> processed(task_count, false);

        double makespan = 0.0;

        for (std::size_t task : scheduling) {

            if (task >= task_count || processed[task])
                return std::nullopt;

            std::size_t machine = matching[task];

            if (machine >= machine_count)
                return std::nullopt;

            double start = std::max(
                machine_free[machine],
                data_ready[task]
            );

            double duration = execution_time_[task][machine];

            if (!std::isfinite(duration) || duration < 0.0)
                return std::nullopt;

            double finish = start + duration;

            if (!std::isfinite(finish))
                return std::nullopt;

            machine_free[machine] = finish;
            makespan = std::max(makespan, finish);

            for (std::size_t next : graph_[task]) {

                if (next >= task_count ||
                    processed[next] ||
                    matching[next] >= machine_count)
                    return std::nullopt;

                double transfer_time = 0.0;

                if (machine != matching[next]) {

                    Transfer transfer{
                        machine,
                        matching[next],
                        output_size_[task]
                    };

                    transfer_time = this->communication_time(transfer);
                }

                double ready = finish + transfer_time;

                if (!std::isfinite(ready))
                    return std::nullopt;

                data_ready[next] = std::max(
                    data_ready[next],
                    ready
                );
            }

            processed[task] = true;
        }

        return makespan;
    }

private:
    const TaskGraph& graph_;
    const std::vector<std::vector<double>>& execution_time_;
    const std::vector<double>& output_size_;
};

}