
#pragma once

#include <optional>
#include <ostream>
#include "concepts/genome.hpp"

namespace ga::core {

template <GenomeType Genome>
struct Chromosome{
    Genome genome;
    std::optional<double> fitness;

    friend std::ostream& operator<<(
        std::ostream& out,
        const Chromosome<Genome>& chromosome
    ) {
        out << "genome: " << chromosome.genome << "\n";

        if (chromosome.fitness){
            out << "fitness: " << *chromosome.fitness << "\n";
        } else {
            out << "fitness not calculated \n";
        }

        return out;
    }
};

}
