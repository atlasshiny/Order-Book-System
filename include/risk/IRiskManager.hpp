#pragma once
#include "orderbook/Order.hpp"

class IRiskManager {
public:
    virtual ~IRiskManager() = default;

    // Method to check if an order is within risk limits
    virtual bool checkOrder(const Order& order) = 0;
};