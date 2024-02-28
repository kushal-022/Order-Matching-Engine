// PriceLevel.h
#ifndef TRADE_H
#define TRADE_H

#include <iostream>
#include <string>

using namespace std;

class Trade {
public:
    int orderId;
    int executeQty;
    int availableQty;
    double executePrice;

public:
    Trade(int orderId, int availableQty, int executeQty, double executePrice) :
        orderId(orderId), executeQty(executeQty), availableQty(availableQty), executePrice(executePrice) {}

    string toString() const{
    return "Trade{orderId=" + to_string(orderId) +
        ", executeQty=" + to_string(executeQty) +
        ", availableQty=" + to_string(availableQty) +
        ", executePrice=" + to_string(executePrice) +
        '}';
    }

};

std::ostream& operator<<(std::ostream& os, const Trade& trade) {
    os << "Trade: "
       << "orderId=" << trade.orderId << ", "
       << "executeQty=" << trade.executeQty << ", "
       << "availableQty=" << trade.availableQty << ", "
       << "executePrice=" << trade.executePrice;
    return os;
}


#endif