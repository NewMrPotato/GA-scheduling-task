
#pragma once

#include <ranges>
#include <iostream>


namespace ga::schedule {

template <typename T>
concept IterableRange = 
    std::ranges::range<T> &&
    std::unsigned_integral<std::ranges::range_value_t<T>>;


template <typename T>
concept Printable = requires(T object){
    {std::cout << object} -> std::same_as<decltype(std::cout)&>;
};

template <typename Genome> 
concept ScheduleGenomeType = 
    Printable<Genome> &&
    requires(const Genome& genome){
        requires IterableRange<decltype(genome.get_scheduling())>;
        requires IterableRange<decltype(genome.get_matching())>;
        
    } &&
    requires(Genome genome) {
        { genome.get_scheduling() } -> std::same_as<std::vector<size_t>>;
        { genome.get_matching() } -> std::same_as<std::vector<size_t>>;
    };

    
template <typename Transfer> 
concept TranferType = 
    requires(std::size_t from,
        std::size_t to,
        double size_data){
            Transfer{from, to, size_data};
        };

}

