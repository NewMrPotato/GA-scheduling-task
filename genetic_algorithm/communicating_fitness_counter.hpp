#pragma once
#include "communication.hpp"
#include "fitness_counter.hpp"
#include <cmath>
#include <stdexcept>

template <typename Genome, typename Transfer>
class CommunicatingFitnessCounter : public FitnessCounter<Genome> {
public:
    CommunicatingFitnessCounter(const Communication<Transfer>& model)
        : communication(model) {}

protected:
    double communication_time(const Transfer& transfer) const {
        double duration = communication.time(transfer);

        if (!std::isfinite(duration) || duration < 0.0)
            throw std::domain_error("Invalid communication duration");
        return duration;
    }

private:

    const Communication<Transfer>& communication;
};
