
#pragma once

#include <cstddef>

struct RunStatistics {
    std::size_t generation = 0;
    std::size_t generations_without_improvement = 0;
    double best_fitness = 0.0;
};
