
#pragma once

#include <cstddef>

namespace ga::schedule{

struct Machine{
    double input_bandwidth;
    double output_bandwidth;
};

struct Transfer{
	std::size_t input_machine;
	std::size_t output_machine;
	std::size_t data_id;
};

}
