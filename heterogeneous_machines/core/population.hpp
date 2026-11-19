
#pragma once

#include <genetic_algorithm/ga.hpp>
#include "genome.hpp"
#include "../concepts/genome.hpp"


namespace ga::schedule {

template <ScheduleGenomeType Genome>
class SchedulePopulation : public ga::core::Population<Genome>{
    
};

}
