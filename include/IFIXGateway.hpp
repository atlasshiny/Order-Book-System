#pragma once
#include "Order.hpp"

class IFIXGateway {
public:
    virtual ~IFIXGateway() = default;

    // Method to send an order to the exchange
    virtual void sendOrder(const Order& order) = 0;

    // Method to cancel an existing order
    virtual void cancelOrder(int orderId) = 0;

    // Method to receive market data updates
    virtual void onMarketDataUpdate() = 0;

    // Method to receive execution reports
    virtual void onExecutionReport() = 0;
};