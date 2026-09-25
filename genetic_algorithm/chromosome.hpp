
#pragma once

#include <optional>

template <typename Genome>
struct Chromosome{
    Genome genome;
    std::optional<double> fitness;
};