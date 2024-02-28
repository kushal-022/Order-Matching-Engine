#ifndef ORDER_INDEX_H
#define ORDER_INDEX_H

#include "EngineConstants.h"
#include "NumberUtil.h"
#include <unordered_map>

class OrderIndex {
public:
    class Index {
    public:
        int bookPriceLevelIndex;
        int priceLevelOrderIndex;

        Index(int pricelevelIndex, int oIdx) {
            bookPriceLevelIndex = pricelevelIndex;
            priceLevelOrderIndex = oIdx;
        }

        Index(int val) {
            bookPriceLevelIndex = NumberUtil::getBookPriceLevelIndex(val);
            priceLevelOrderIndex = NumberUtil::getPriceLevelOrderIndex(val);
        }

        int coalesceValue() const{
            return NumberUtil::coalescePriceAndOrderLevelIndex(bookPriceLevelIndex, priceLevelOrderIndex);
        }
    };

private:
    std::unordered_map<int, int> orderIndex;

public:
    OrderIndex() {
        orderIndex.reserve((1<<15) / 4);
    }

    void addIndex(int orderId, const Index& idx) {
        orderIndex[orderId] = idx.coalesceValue();
    }


    Index getIndex(int orderId) {
        int val = orderIndex[orderId];
        return Index(val);
    }

    Index removeIndex(int orderId) {
        int val = orderIndex[orderId];
        orderIndex.erase(orderId);
        return Index(val);
    }
};

#endif // ORDER_INDEX_H
