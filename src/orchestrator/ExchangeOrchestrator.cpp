#include "orchestrator/ExchangeOrchestrator.hpp"

void ExchangeOrchestrator::processRawMessage(std::string_view rawWireMsg) {
        
        // Use the chosen protocol gateway to parse the wire bytes
        std::optional<Order> parsedOrderOpt = gateway_->receiveOrder(rawWireMsg);
        if (!parsedOrderOpt.has_value()) {
            return; // Parser rejected or malformed data
        }

        Order& engineOrder = parsedOrderOpt.value();

        // Run high-speed concrete pre-trade risk checks
        if (!riskManager_.checkOrder(engineOrder)) {
            return; 
        }
    
        // Process inside the concrete matching engine
        if (engineOrder.type == OrderType::LIMIT) {
            orderBook_.placeLimitOrder(engineOrder);
        } else {
            orderBook_.placeMarketOrder(engineOrder);
        }

        orderBook_.matchOrders();
    };