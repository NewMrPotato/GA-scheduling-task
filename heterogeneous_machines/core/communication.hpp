
#pragma once

#include <genetic_algorithm/ga.hpp>
#include "genome.hpp"
#include "machine.hpp"
#include <algorithm>
#include <cmath>
#include <stdexcept>
#include "../concepts/genome.hpp"


namespace ga::schedule {


struct Transfer{
    std::size_t from_machine;
    std::size_t to_machine;
    double data_size;
};


template <TranferType Transfer>
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
		if (!std::isfinite(communication_bandwidth) || communication_bandwidth <= 0.0 ||
			!std::isfinite(latency) || latency < 0.0) {
			throw std::invalid_argument("Invalid network parameters");
		}
    }

    double time(const Transfer& transfer) const override {
		if (transfer.from_machine >= machines.size() || transfer.to_machine >= machines.size() ||
			!std::isfinite(transfer.data_size) || transfer.data_size < 0.0) {
			throw std::invalid_argument("Invalid transfer");
		}
		if (transfer.from_machine == transfer.to_machine || transfer.data_size == 0.0) {
			return 0.0;
		}

		double output_bandwidth = machines[transfer.from_machine].get_out_bandwidth();
		double input_bandwidth = machines[transfer.to_machine].get_in_bandwidth();
		if (!std::isfinite(output_bandwidth) || output_bandwidth <= 0.0 ||
			!std::isfinite(input_bandwidth) || input_bandwidth <= 0.0) {
			throw std::invalid_argument("Machine bandwidth must be finite and positive");
		}

        double bandwidth = std::min({
            communication_bandwidth,
            output_bandwidth,
            input_bandwidth
        });
		return latency + transfer.data_size / bandwidth;
    }
    
};

}
