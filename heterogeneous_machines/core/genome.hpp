#pragma once

#include <vector>


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
};

}
