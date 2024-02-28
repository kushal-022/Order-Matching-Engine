// PriceLevelComparator.h
#ifndef PRICE_LEVEL_COMPARATOR_H
#define PRICE_LEVEL_COMPARATOR_H

#include <functional> 

enum class Side {
    BUY,
    SELL
};

class PriceLevelComparator {
private:
    const Side side;

public:
    PriceLevelComparator(Side side) : side(side) {}

    auto operator()(double o1, double o2) const {
        return [this](double o1, double o2) {
            return side == Side::SELL ? o1 < o2 : o2 < o1;
        };
    }
};

#endif // PRICE_LEVEL_COMPARATOR_H
