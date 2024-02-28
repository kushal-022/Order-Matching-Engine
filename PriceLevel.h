// PriceLevel.h
#ifndef PRICE_LEVEL_H
#define PRICE_LEVEL_H

#include <iostream>
#include <string>
#include <sstream>
#include "EngineConstants.h"
#include "Trade.h"
#include "Side.h"

using namespace std;

class PriceLevel {
public:
    double price;
    const Side side;
    int totalOpenOrders = 0;
    int totalCancelOrders = 0;
    int totalExecuteOrders = 0;
    int start = -1;
    int end = -1;
    int orderQuantity[1 << 15][2];

public:
    PriceLevel(Side s) : side(s) {} // Constructor to initialize side

    PriceLevel(const PriceLevel& other) : price(other.price), side(other.side), totalOpenOrders(other.totalOpenOrders),
                                           totalCancelOrders(other.totalCancelOrders), totalExecuteOrders(other.totalExecuteOrders),
                                           start(other.start), end(other.end) {
        // Copy the contents of orderQuantity
        for (int i = 0; i < (1 << 15); ++i) {
            orderQuantity[i][0] = other.orderQuantity[i][0];
            orderQuantity[i][1] = other.orderQuantity[i][1];
        }
    }

    PriceLevel& operator=(const PriceLevel& other) {
        if (this != &other) {
            price = other.price;
            // side = other.side; // No need to assign const member
            totalOpenOrders = other.totalOpenOrders;
            totalCancelOrders = other.totalCancelOrders;
            totalExecuteOrders = other.totalExecuteOrders;
            start = other.start;
            end = other.end;
            // Copy the contents of orderQuantity
            for (int i = 0; i < (1 << 15); ++i) {
                orderQuantity[i][0] = other.orderQuantity[i][0];
                orderQuantity[i][1] = other.orderQuantity[i][1];
            }
        }
        return *this;
    }

    string toString() const {
        return "PriceLevel{" +
               to_string(price) +
               ", totalOpenOrders=" + to_string(totalOpenOrders) +
               ", totalCancelOrders=" + to_string(totalCancelOrders) +
               ", totalExecuteOrders=" + to_string(totalExecuteOrders) +
               ", orders= " + orderInfo() +
               "}";
    }

    private :

    int moduloPowerOfTwo(int v, int d) const {
        return v & (d - 1);
    }

    string orderInfo() const{
        int pos = start;
        if(pos == -1) return "No Orders";
        ostringstream sB;
        sB << " [ ";
        do{
            if(!isInvalid(pos)){
                sB << orderQuantity[pos][0] << "=" << orderQuantity[pos][1] << " | ";
            }
            if(pos == end) break;
            pos = moduloPowerOfTwo(pos+1, 1<<15);
        } while(true);
        sB << " ]";
        return sB.str();
    }

    public :

    bool isEmpty() const{ return totalOpenOrders ==0 ;}

    int newOrder(int orderId, Order order){
        if(totalOpenOrders == 0){
            reset(order.getPrice());
        }
        if(start == -1){
            start = 0;
            int idx = start;
            insertOrder(orderId, order, idx);
            return idx;
        }

        const int idx = moduloPowerOfTwo(end + 1, 1<<15);
        if(idx == start){
            throw runtime_error("All available entries at price level are allotted");
        }
        insertOrder(orderId, order, idx);
        return idx;
    }

    void insertOrder(int orderId, Order order, int idx){
        orderQuantity[idx][0] = orderId;
        orderQuantity[idx][1] = order.getQuantity();
        end = idx;
        totalOpenOrders++;
    }

    void reset(double price){
        this->price = price ;
        this->totalOpenOrders = this->totalCancelOrders = this->totalExecuteOrders = 0;
        this->start = this->end = -1;
    }

    public:

    bool cancelOrder(int orderId, int oIdx){
        if(orderQuantity[oIdx][0] != orderId){
            return false;
        }
        totalCancelOrders++;
        makeInvalid(oIdx);
        adjustIndex(oIdx);
        return true;
    }

    private :

    void makeInvalid(int oIdx){
        orderQuantity[oIdx][0] = -1;
        orderQuantity[oIdx][1] = -1;
        totalOpenOrders--;
    }
    
    void adjustIndex(int oIdx) {
        if (start == oIdx && end == oIdx) {
            reset(0);
            return;
        }

        if (oIdx == start) {
            do {
                // shift the start to traverse all closed orders
                start = moduloPowerOfTwo((start + 1), 1<<15);
            } while (start != end && isInvalid(start));
        }
        if (oIdx == end) {
            do {
                // shift the end to traverse all closed orders
                end = end > 0 ? end - 1 : (1<<15) - 1;
            } while (end != start && isInvalid(end));
        }

        if (start == end && isInvalid(start)) {
            reset(0);
        }
        return;
    }

    public :
    
    bool isInvalid(int idx) const{
        return orderQuantity[idx][0] <= 0 && orderQuantity[idx][1] <= 0;
    }

    double getPrice() const{
        return price;
    }

    public :

    void matchOrder(int orderId, Order order, vector<Trade>& trades) {
        int pos = start;
        if (pos == -1) return;
        do {
            if (!isInvalid(pos)) {
                totalExecuteOrders++;
                const int crossOrderId = orderQuantity[pos][0];
                const int crossQty = orderQuantity[pos][1];
                const int orderQty = order.getQuantity();
                if (orderQty >= crossQty) {
                    trades.push_back(Trade(crossOrderId, 0, crossQty, price));
                    trades.push_back(Trade(orderId, orderQty - crossQty, crossQty, price));
                    order.decreaseQty(crossQty);
                    makeInvalid(pos);
                } else {
                    trades.push_back(Trade(crossOrderId, crossQty - orderQty, orderQty, price));
                    trades.push_back(Trade(orderId, 0, orderQty, price));
                    orderQuantity[pos][1] -= orderQty;
                    order.decreaseQty(orderQty);
                    break;
                }
            }
            if (pos == end) break;
            pos = moduloPowerOfTwo(pos + 1, 1<<15);
        } while (true);

        resetStartEnd();
    }

    private:

    void resetStartEnd() {
        if (isInvalid(start)) {
            // Shift the start to traverse all closed orders
            while (start != end && isInvalid(start)) {
                start = moduloPowerOfTwo((start + 1), 1<<15);
            }
        }
        if (isInvalid(end)) {
            // Shift the end to traverse all closed orders
            while (end != start && isInvalid(end)) {
                end = end > 0 ? end - 1 : (1<<15) - 1;
            }
        }
        if (start == end && isInvalid(start)) {
            reset(0);
        }
    }

};

#endif