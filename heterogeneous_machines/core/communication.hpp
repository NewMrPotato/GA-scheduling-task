
#pragma once

#include <genetic_algorithm/ga.hpp>
#include "genome.hpp"
#include <algorithm>


namespace ga::schedule {


struct Transfer{
    std::size_t from_machine;
    std::size_t to_machine;
    double data_size;
};


template <typename Transfer>
class ScheduleCommunication : public ga::core::Communication<Transfer>{

private:
    const std::vector<Machine>& machines;
    double communication_bandwidth;
    double latency;

public:    
    ScheduleCommunication(
        const std::vector<Machine>& machines, 
        double communication_bandwidth, 
        double latency) :
    machines(machines), 
    communication_bandwidth(communication_bandwidth), 
    latency(latency) {

    }

    double time(const Transfer& transfer) const{
        return latency + std::min({
            communication_bandwidth,
            machines[transfer.from_machine].get_out_bandwidth(),
            machines[transfer.to_machine].get_in_bandwidth()
        });
    }
    
};

}
