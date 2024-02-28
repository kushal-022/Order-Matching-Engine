#ifndef ORDER_H
#define ORDER_H
#include "Side.h"

class Order {
private:
    int quantity;
    double price;
    Side side;

public:
    Order(int quantity, double price, Side side) : quantity(quantity), price(price), side(side) {}

    int getQuantity() const {
        return quantity;
    }

    double getPrice() const {
        return price;
    }

    Side getSide() const {
        return side;
    }

    bool isBuy() const {
        return side == Side::BUY;
    }

    bool canExecute(double oppositePrice) const {
        return (isBuy() && price >= oppositePrice) || (!isBuy() && oppositePrice >= price);
    }

    void decreaseQty(int crossQty) {
        quantity -= crossQty;
    }

    bool isComplete() const {
        return quantity == 0;
    }
};

#endif
