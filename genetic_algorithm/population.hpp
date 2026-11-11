
#pragma once

#include <vector>
#include <cstddef>
#include "chromosome.hpp"

namespace ga::core {

template <typename Genome>
class Population{
    private:
    std::vector<Chromosome<Genome>> chromosomes;

    public:
    Population() = default;

    Population(const std::vector<Chromosome<Genome>>& initial)
        : chromosomes(initial) {}

    void add(const Chromosome<Genome>& chromosome) { chromosomes.push_back(chromosome); }
    bool empty() const { return chromosomes.empty(); }

    Chromosome<Genome>& operator[](std::size_t index) {
        return chromosomes[index];
    }

    const Chromosome<Genome>& operator[](std::size_t index) const{
        return chromosomes[index];
    }

    std::size_t size() const{
        return chromosomes.size();
    }

    auto begin() { return chromosomes.begin(); }
    auto end() { return chromosomes.end(); }

    auto begin() const { return chromosomes.begin(); }
    auto end() const { return chromosomes.end(); }
};

}
