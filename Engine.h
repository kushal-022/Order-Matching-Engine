#ifndef ENGINE_H
#define ENGINE_H

#include "IEngine.h"
#include "Order.h"
#include "OrderIndex.h"
#include "OrderBook.h"
#include "Trade.h"
#include <iostream>
#include <vector>

class Engine : public IEngine {
private:
    int orderIdGenerator = 0;
    OrderIndex buyOrderIndex;
    OrderIndex sellOrderIndex;
    OrderBook buyOrderBook;
    OrderBook sellOrderBook;

public:
    Engine() : buyOrderBook(Side::BUY), sellOrderBook(Side::SELL) {}

    int limit(const Order& order) override {
        const int orderId = ++orderIdGenerator;
        if (order.isBuy()) {
            matchAndInsert(sellOrderBook, orderId, order, buyOrderBook, buyOrderIndex);
        } else {
            matchAndInsert(buyOrderBook, orderId, order, sellOrderBook, sellOrderIndex);
        }
        return orderId;
    }

    bool cancel(int orderId) override {
        OrderIndex::Index buyIndex = buyOrderIndex.removeIndex(orderId);
        if (buyIndex.bookPriceLevelIndex != -1) {
            return buyOrderBook.onCancelOrder(orderId, buyIndex);
        }
        OrderIndex::Index sellIndex = sellOrderIndex.removeIndex(orderId);
        if (sellIndex.bookPriceLevelIndex != -1) {
            return sellOrderBook.onCancelOrder(orderId, sellIndex);
        }
        return false;
    }

    void execute(int orderId, Side side, const std::vector<Trade>& trades) {
        std::cout << "==== TRADE ===" << std::endl;
        for (const Trade& trade : trades) {
            cout<<trade <<endl;
            if (trade.availableQty > 0 || trade.orderId == orderId) {
                continue;
            }
            if (side == Side::BUY) {
                sellOrderIndex.removeIndex(trade.orderId);
            } else {
                buyOrderIndex.removeIndex(trade.orderId);
            }
        }
        std::cout << "===============" << std::endl;
    }

    std::string toString() const {
        return "Engine{,\n sellOrderBook=" + sellOrderBook.toString() +
               "buyOrderBook=" + buyOrderBook.toString() +
               '}';
    }
    
    void runDemo() {
        Order order0(100, 10.0, Side::BUY);
        Order order1(200, 11.0, Side::BUY);
        Order order2(50, 10.0, Side::BUY);
        Order order3(300, 12.0, Side::BUY);
        Order order4(400, 11.0, Side::BUY);
        Order order5(50, 13.0, Side::BUY);
        int o0 = limit(order0);
        int o1 = limit(order1);
        int o2 = limit(order2);
        int o3 = limit(order3);
        int o4 = limit(order4);
        int o5 = limit(order5);

        Order sorder0(400, 15.0, Side::SELL);
        Order sorder1(100, 16.0, Side::SELL);
        Order sorder2(250, 15.0, Side::SELL);
        Order sorder3(50, 17.0, Side::SELL);
        Order sorder4(200, 16.0, Side::SELL);
        Order sorder5(300, 18.0, Side::SELL);
        int so0 = limit(sorder0);
        int so1 = limit(sorder1);
        int so2 = limit(sorder2);
        int so3 = limit(sorder3);
        int so4 = limit(sorder4);
        int so5 = limit(sorder5);

        std::cout << "=======ENGINE=========" << std::endl;
        std::cout << toString() << std::endl;

        cancel(o2);
        cancel(o5);

        std::cout << "=======ENGINE=========" << std::endl;
        std::cout << toString() << std::endl;

        Order order6(300, 14.0, Side::BUY);
        Order order7(100, 13.0, Side::BUY);
        int o6 = limit(order6);
        int o7 = limit(order7);

        std::cout << "=======ENGINE=========" << std::endl;
        std::cout << toString() << std::endl;

        cancel(o0);
        cancel(o1);
        cancel(o3);
        cancel(o4);
        cancel(o7);

        std::cout << "=======ENGINE=========" << std::endl;
        std::cout << toString() << std::endl;

        cancel(so1);
        cancel(so3);
        cancel(so5);
        cancel(o6);

        std::cout << "=======ENGINE=========" << std::endl;
        std::cout << toString() << std::endl;

        Order order8(900, 16.0, Side::BUY);
        int o8 = limit(order8);

        std::cout << "=======ENGINE=========" << std::endl;
        std::cout << toString() << std::endl;
    }

private:
    void matchAndInsert(OrderBook& crossBook, int orderId, const Order& order, OrderBook& orderBook, OrderIndex& orderIndex) {
        std::vector<Trade> trades = crossBook.matchOrder(orderId, order);
        if (!trades.empty()) {
            execute(orderId, order.getSide(), trades);
        }
        if (!order.isComplete()) {
            OrderIndex::Index buyindex = orderBook.onNewOrder(orderId, order);
            orderIndex.addIndex(orderId, buyindex);
        }
    }

};

#endif // ENGINE_H
