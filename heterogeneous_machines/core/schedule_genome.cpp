
#include <vector>

class ScheduleGenome{
private: 
    std::vector<int> matching;
    std::vector<int> scheduling;

public:
    ScheduleGenome(
        std::vector<int> matching,
        std::vector<int> scheduling)
        : matching(matching), scheduling(scheduling) {}
};