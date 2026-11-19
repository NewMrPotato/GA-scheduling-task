#pragma once

#include <vector>
#include "../concepts/genome.hpp"


namespace ga::schedule {

template <ScheduleGenomeType Genome>
class ScheduleChromosome : public ga::core::Chromosome<Genome>{
        
};

}
