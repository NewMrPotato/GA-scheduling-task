
#include <population.hpp>

template <typename Genome>
struct Mutation{
    virtual mutate(Population&<Genome> population) = 0;
};