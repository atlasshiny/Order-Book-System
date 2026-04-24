#pragma once
#include <deque>
#include "Order.hpp"

class OrderBook {
private:
    std::deque<Order> bidOrders;
    std::deque<Order> askOrders;
public:
    void addOrder(const Order& order);
    void matchOrders();
};
