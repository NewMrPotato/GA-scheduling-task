
#pragma once

#include <ranges>
#include <iostream>


namespace ga::core {

template <typename Genome>
concept GenomeType =
    std::copy_constructible<Genome> &&
    std::movable<Genome>;

}

