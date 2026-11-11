#pragma once

#include <vector>
#include <ostream>


namespace ga::schedule {

class ScheduleGenome{
private:
    std::vector<size_t> matching;
    std::vector<size_t> scheduling;

public:
    ScheduleGenome(
        std::vector<size_t> matching,
        std::vector<size_t> scheduling)
        : matching(matching), scheduling(scheduling) {}

    std::vector<size_t> get_matching() const{
        return matching;
    }

    std::vector<size_t> get_scheduling() const{
        return scheduling;
    }

};
    std::ostream& operator<<(
        std::ostream& out,
        const ScheduleGenome& genome
    ) {
        out << "\n";
        out << "[";
        for (const auto& value: genome.get_matching()){
            out << value << ", ";
        }
        out << "]\n";

        out << "[";
        for (const auto& value: genome.get_scheduling()){
            out << value << ", ";
        }
        out << "]\n";
        
        return out;
    }

}
