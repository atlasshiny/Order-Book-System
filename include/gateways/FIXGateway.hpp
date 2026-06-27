#pragma once
#include "gateways/IGateway.hpp"
#include "orderbook/Order.hpp"
#include "parsers/FIXParser.hpp"
#include "writers/FIXWriter.hpp"

class FIXGateway : public IGateway {
private:
    FIXWriter fixWriter_;
    FIXParser fixParser_;

public:
    size_t sendOrder(const Order& order, char* wireBuffer_, size_t bufferSize);

    std::optional<Order> receiveOrder(std::string_view rawData);

    void cancelOrder(int orderId);
    
    void onMarketDataUpdate();

    void onExecutionReport();
};