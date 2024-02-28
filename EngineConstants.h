// EngineConstants.h

#ifndef ENGINE_CONSTANTS_H
#define ENGINE_CONSTANTS_H

class EngineConstants {
public:
    static const int MAX_PRICE_LEVELS = 1 << 8;   // 256
    static const int MAX_ORDERS_AT_EACH_PRICE_LEVEL = 1 << 15;  // 32768
    static const int MAX_ORDERS_IN_BOOK = 1 << 15; // 32768
};

#endif
