#pragma once
#include <deque>
#include "Order.hpp"

class ExchangeOrchestrator; // Forward declaration

class OrderBook {
private:
    std::deque<Order> bidOrders;
    std::deque<Order> askOrders;
    int nextOrderId = 1;
    void matchAgainstBook(Order& incomingOrder, std::deque<Order>& oppositeBook);

    // Orchestrator pointer for callbacks
    ExchangeOrchestrator* orchestrator_ = nullptr;

public:
    // Core functionalities
    void placeLimitOrder(Order& order);
    void placeMarketOrder(Order& order);
    void placeImmediateOrCancelOrder(Order& order);
    void placePostOnlyOrder(Order& order);
    void matchOrders();
    void cancelOrder(int orderId);

    // Market data retrieval
    void level1Data() const;
    void level2Data() const;

    // Orchestrator management
    void set_orchestrator(ExchangeOrchestrator* orchestrator);
    void remove_orchestrator();
};
