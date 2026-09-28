
#pragma once

template <typename Transfer>
struct Communication {
    virtual double time(const Transfer& transfer) const = 0;

    virtual ~Communication() = default;
};
