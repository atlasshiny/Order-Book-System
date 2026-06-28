#include "orchestrator/ExchangeOrchestrator.hpp"
#include <iostream>

void ExchangeOrchestrator::processOrder(Order& order) {
    // Run high-speed concrete pre-trade risk checks
    if (!riskManager_.checkOrder(order)) {
        std::cout << "Order rejected by risk manager." << std::endl; // I/O blocking
        return; 
    }
    
    // Process inside the concrete matching engine
    if (order.type == OrderType::LIMIT) {
        orderBook_.placeLimitOrder(order);
        std::cout << "Limit order parsed and placed successfully." << std::endl; // I/O blocking
    } else {
        orderBook_.placeMarketOrder(order);
        std::cout << "Market order parsed and executed successfully." << std::endl; // I/O blocking
    }

    orderBook_.matchOrders();
};

std::optional<Order> ExchangeOrchestrator::on_data_received(std::shared_ptr<TCPSession> session, std::string_view raw_data) {
    return gateway_->on_data_received(session, raw_data);
}

void ExchangeOrchestrator::on_client_connect(std::shared_ptr<TCPSession> session) {
    gateway_->on_client_connect(session);
}

void ExchangeOrchestrator::on_client_disconnect(std::shared_ptr<TCPSession> session) {
    gateway_->on_client_disconnect(session);
}

void ExchangeOrchestrator::outputOrderBookState() const {
        std::cout << "\nLevel 1 Data:" << std::endl;
        orderBook_.level1Data();

        std::cout << "\nLevel 2 Data:" << std::endl;
        orderBook_.level2Data();
};