
#pragma once

#include <optional>
#include <ostream>

namespace ga::core {

template <typename Genome>
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
