
	#pragma once

	#include <cstddef>

	namespace ga::schedule{

	class Machine{
	private:
		double input_bandwidth;
		double output_bandwidth;

	public:
		Machine(double input_bandwidth, double output_bandwidth) :
		input_bandwidth(input_bandwidth),
		output_bandwidth(output_bandwidth){
		}

		double get_in_bandwidth() const {
			return input_bandwidth;
		}

		double get_out_bandwidth() const {
			return output_bandwidth;
		}
	};

	}
