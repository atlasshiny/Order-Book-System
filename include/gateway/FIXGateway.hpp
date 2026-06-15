#pragma once
#include "IFIXGateway.hpp"

class FIXGateway : public IFIXGateway {
public:
    void sendOrder(const Order& order) override {}

    void cancelOrder(int orderId) override {}

    void onMarketDataUpdate() override {}

    void onExecutionReport() override {}
};