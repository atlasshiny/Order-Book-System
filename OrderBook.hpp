#pragma once
#include <deque>
#include "Order.hpp"

class OrderBook {
private:
    std::deque<Order> bidOrders;
    std::deque<Order> askOrders;
    int nextOrderId = 1;
public:
    void placeLimitOrder(Order& order);
    void placeMarketOrder(Order& order);
    void matchOrders();
    void cancelOrder(int orderId);
    void level1Data() const;
    void level2Data() const;
};
