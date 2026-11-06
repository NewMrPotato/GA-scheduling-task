
#pragma once

#include <optional>

namespace ga::core {

template <typename Genome>
struct Chromosome{
    Genome genome;
    std::optional<double> fitness;
};

}
