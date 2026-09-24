
#include <population.hpp>

template <typename Genome>
struct Selection{
    virtual select(Population&<Genome> population) = 0;
};