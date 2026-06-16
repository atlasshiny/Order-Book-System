#pragma once
#include <deque>
#include "Order.hpp"

class OrderBook {
private:
    std::deque<Order> bidOrders;
    std::deque<Order> askOrders;
    int nextOrderId = 1;
    void matchAgainstBook(Order& incomingOrder, std::deque<Order>& oppositeBook);
public:
    // Core functionalities
    void placeLimitOrder(Order& order);
    void placeMarketOrder(Order& order);
    void placeImmediateOrCancelOrder(Order& order);
    void placePostOnlyOrder(Order& order);
    void matchOrders();
    void cancelOrder(int orderId);
    void onNewOrder(const Order& order); // Callback for new orders

    // Market data retrieval
    void level1Data() const;
    void level2Data() const;
};
