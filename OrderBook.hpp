#pragma once
#include <deque>
#include "Order.hpp"

class OrderBook {
private:
    std::deque<Order> bidOrders;
    std::deque<Order> askOrders;
public:
    void placeLimitOrder(const Order& order);
    void placeMarketOrder(const Order& order);
    void matchOrders();
};
