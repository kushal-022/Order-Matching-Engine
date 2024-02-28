#ifndef ORDER_BOOK_H
#define ORDER_BOOK_H

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <algorithm>
#include "Order.h"
#include "Trade.h"
#include "OrderIndex.h"
#include "PriceLevel.h"
#include "EngineConstants.h"

using namespace std;

class OrderBook {
private:
    Side side;
    int start = -1;
    int end = -1;
    vector<PriceLevel> priceLevels;
    unordered_map<double, int> priceLevelIndexMap;

public:
    OrderBook(Side side) : side(side) {
        priceLevels.reserve(1 << 8);
        for (int i = 0; i < (1 << 8); ++i) {
            priceLevels.emplace_back(side);
        }
    }

    vector<Trade> matchOrder(int orderId, const Order& order) {
        if ((side == Side::BUY && order.isBuy()) || (side == Side::SELL && !order.isBuy()))
            return vector<Trade>();

        if (priceLevelIndexMap.empty())
            return vector<Trade>();

        vector<Trade> trades;
        do {
            auto entry = priceLevelIndexMap.begin();
            double price = entry->first;
            int index = entry->second;

            if (!order.canExecute(price))
                return trades;

            PriceLevel& priceLevel = priceLevels[index];
            priceLevel.matchOrder(orderId, order, trades);

            if (priceLevel.isEmpty())
                priceLevelIndexMap.erase(price);

            if (order.isComplete())
                return trades;

        } while (!priceLevelIndexMap.empty());

        return trades;
    }

    OrderIndex::Index onNewOrder(int orderId, const Order& order) {
        double price = order.getPrice();
        int priceLevelIdx = priceLevelIndexMap.find(price) != priceLevelIndexMap.end() ? priceLevelIndexMap[price] : -1;

        if (priceLevelIdx != -1)
            return newOrder(orderId, order, priceLevelIdx);
        else {
            if (start == -1) {
                start = end = 0;
                int idx = start;
                priceLevelIndexMap[price] = idx;
                return newOrder(orderId, order, idx);
            }

            int idx = (end + 1) % (1<<8);
            if (idx == start)
                throw runtime_error("All available price levels are allotted");

            end = idx;
            priceLevelIndexMap[price] = idx;
            return newOrder(orderId, order, idx);
        }
    }

    bool onCancelOrder(int orderId, const OrderIndex::Index& index) {
        int pIdx = index.bookPriceLevelIndex;
        int oIdx = index.priceLevelOrderIndex;

        PriceLevel& priceLevel = priceLevels[pIdx];
        double price = priceLevel.getPrice();

        bool success = priceLevel.cancelOrder(orderId, oIdx);

        if (!success)
            return false;

        if (priceLevel.isEmpty())
            priceLevelIndexMap.erase(price);

        adjustIndexes(pIdx, oIdx);
        return true;
    }

    string toString() const {
        string result = "OrderBook{\n";
        result += "side=" + to_string(static_cast<int>(side)) + "\n";
        result += "priceLevels = \n" + priceLevelInfo() + "\n}\n";
        return result;
    }

private:
    OrderIndex::Index newOrder(int orderId, const Order& order, int priceLevelIdx) {
        PriceLevel& priceLevel = priceLevels[priceLevelIdx];
        int oIdx = priceLevel.newOrder(orderId, order);
        return OrderIndex::Index(priceLevelIdx, oIdx);
    }

    string priceLevelInfo() const {
        if (start == -1)
            return "No price levels";

        string result;
        int pos = start;
        do {
            double price = priceLevels[pos].getPrice();
            int idx = priceLevelIndexMap.at(price);

            if (!priceLevels[idx].isEmpty())
                result += priceLevels[idx].toString() + "\n";

            pos = (pos + 1) % (1<<8);

        } while (pos != end && !priceLevels[pos].isEmpty());

        return result;
    }

    void adjustIndexes(int pIdx, int oIdx) {
        if (pIdx == start && pIdx == end) {
            reset();
            return;
        }

        if (pIdx == start) {
            do {
                start = (start + 1) % (1 << 8);
            } while (start != end && priceLevels[start].isEmpty());
        }

        if (oIdx == end) {
            do {
                end = (end > 0) ? end - 1 : (1 << 8) - 1;
            } while (end != start && priceLevels[end].isEmpty());
        }

        if (start == end && priceLevels[start].isEmpty())
            reset();
    }

    void reset() {
        start = end = -1;
    }
};

#endif
