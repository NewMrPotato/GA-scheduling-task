
#include <population.hpp>

template <typename Genome>
struct Crossover{
    virtual crossover(Population&<Genome> population) = 0;
};