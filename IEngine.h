#ifndef IENGINE_H
#define IENGINE_H

#include "Order.h"

class IEngine {
public:
    virtual int limit(const Order& order) = 0;
    virtual bool cancel(int orderId) = 0;
};

#endif
