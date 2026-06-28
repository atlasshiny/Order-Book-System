#pragma once
#include "orderbook/OrderBook.hpp"
#include "risk/SimpleRiskManager.hpp"
#include "gateways/IGateway.hpp"
#include "orderbook/Order.hpp" 
#include "gateways/FIXDefinition.hpp"
#include <memory>

class ExchangeOrchestrator {
public:
    // Constructor accepts which protocol gateway we want to use
    ExchangeOrchestrator(std::unique_ptr<IGateway> protocolGateway)
        : gateway_(std::move(protocolGateway)), riskManager_(), orderBook_() {}

    // Main execution pipeline entry point
    void processRawMessage(std::string_view rawWireMsg);

private:
    std::unique_ptr<IGateway> gateway_; // Abstract boundary for protocol decoding
    SimpleRiskManager riskManager_; 
    OrderBook orderBook_; 
};