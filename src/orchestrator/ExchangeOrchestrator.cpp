#include "orchestrator/ExchangeOrchestrator.hpp"
#include <iostream>

void ExchangeOrchestrator::processRawMessage(std::string_view rawWireMsg) {
        
    // Use the chosen protocol gateway to parse the wire bytes
    std::optional<Order> parsedOrderOpt = gateway_->receiveOrder(rawWireMsg);
    if (!parsedOrderOpt.has_value()) {
        std::cout << "Gateway Error: FIXParser rejected or failed to parse the wire message." << std::endl;
        return; // Parser rejected or malformed data
    }

    Order& engineOrder = parsedOrderOpt.value();

    // Run high-speed concrete pre-trade risk checks
    if (!riskManager_.checkOrder(engineOrder)) {
        std::cout << "Order rejected by risk manager." << std::endl; // I/O blocking
        return; 
    }
    
    // Process inside the concrete matching engine
    if (engineOrder.type == OrderType::LIMIT) {
        orderBook_.placeLimitOrder(engineOrder);
        std::cout << "Limit order parsed and placed successfully." << std::endl; // I/O blocking
    } else {
        orderBook_.placeMarketOrder(engineOrder);
        std::cout << "Market order parsed and executed successfully." << std::endl; // I/O blocking
    }

    orderBook_.matchOrders();
};

void ExchangeOrchestrator::outputOrderBookState() const {
        std::cout << "\nLevel 1 Data:" << std::endl;
        orderBook_.level1Data();

        std::cout << "\nLevel 2 Data:" << std::endl;
        orderBook_.level2Data();
};