#pragma once
#include "orderbook/Order.hpp"

class FIXGateway {
public:
    void sendOrder(const Order& order);

    void cancelOrder(int orderId);
    
    void onMarketDataUpdate();

    void onExecutionReport();
};