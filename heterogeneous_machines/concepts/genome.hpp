
#pragma once

#include <ranges>
#include <iostream>


namespace ga::schedule {

template <typename T>
concept Iterable = std::ranges::range<T>;

template <typename T>
concept Printable = requires(T object){
    {std::cout << object} -> std::same_as<decltype(std::cout)&>;
};

template <typename Genome> 
concept ScheduleGenomeType = 
    Printable<Genome> &&
    requires(Genome genome) {
        { genome.get_scheduling() } -> std::same_as<std::vector<size_t>>;
        { genome.get_matching() } -> std::same_as<std::vector<size_t>>;
    };


}

