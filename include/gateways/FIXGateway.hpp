#pragma once
#include "gateways/IGateway.hpp"
#include "orderbook/Order.hpp"
#include "parsers/FIXParser.hpp"
#include "writers/FIXWriter.hpp"

class FIXGateway : public IGateway {
private:
    FIXWriter fixWriter_;
    FIXParser fixParser_;
    char wireBuffer_[1024]; // temporary buffer for FIX message serialization/deserialization (would eventually be the network socket buffer)

public:
    void sendOrder(const Order& order);

    void receiveOrder(const char* rawData, size_t dataSize);

    void cancelOrder(int orderId);
    
    void onMarketDataUpdate();

    void onExecutionReport();
};