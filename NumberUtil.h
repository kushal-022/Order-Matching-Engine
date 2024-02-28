#ifndef NUMBER_UTIL_H
#define NUMBER_UTIL_H

#include "EngineConstants.h"

class NumberUtil {
public:
    static int moduloPowerOfTwo(int v, int d) {
        return v & (d - 1);
    }

    static int coalescePriceAndOrderLevelIndex(int bookPriceLevelIndex, int priceLevelOrderIndex) {
        return (bookPriceLevelIndex << 15) | priceLevelOrderIndex;
    }

    static int getPriceLevelOrderIndex(int coalescedVal) {
        return coalescedVal & (1<<15 - 1);
    }

    static int getBookPriceLevelIndex(int coalescedVal) {
        return (coalescedVal >> 15) & (1<<8 - 1);
    }
};

#endif // NUMBER_UTIL_H
