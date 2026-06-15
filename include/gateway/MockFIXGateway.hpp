#pragma once
#include "IFIXGateway.hpp"
#include "orderbook/Order.hpp"
#include <iostream>

class MockFIXGateway : public IFIXGateway {
public:
    void sendOrder(const Order& order) override {
        // Mock implementation: Print the order details
        std::cout << "MockFIXGateway: Sending order - ID: " << order.id
                  << ", Type: " << (order.type == OrderType::LIMIT ? "LIMIT" : "MARKET")
                  << ", Quantity: " << order.quantity
                  << ", Price: " << order.price << std::endl;
    }

    void cancelOrder(int orderId) override {
        // Mock implementation: Print the cancellation request
        std::cout << "MockFIXGateway: Cancelling order with ID: " << orderId << std::endl;
    }

    void onMarketDataUpdate() override {
        // Mock implementation: Print a market data update message
        std::cout << "MockFIXGateway: Received market data update." << std::endl;
    }

    void onExecutionReport() override {
        // Mock implementation: Print an execution report message
        std::cout << "MockFIXGateway: Received execution report." << std::endl;
    }
};