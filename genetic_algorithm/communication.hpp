
#pragma once

namespace ga::core {

template <typename Transfer>
struct Communication {

    virtual double time(const Transfer& transfer) const = 0;

    virtual ~Communication() = default;
};

}
