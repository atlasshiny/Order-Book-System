#pragma once
#include <vector>
#include "Order.hpp"

class OrderBook {
private:
    std::vector<Order> bidOrders;
    std::vector<Order> askOrders;
public:
    void addOrder(const Order& order);
    void matchOrders();
};
