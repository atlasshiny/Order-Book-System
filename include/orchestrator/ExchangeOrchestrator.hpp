#pragma once
#include "orderbook/OrderBook.hpp"
#include "risk/RiskManager.hpp"
#include "gateways/IGateway.hpp"
#include "orderbook/Order.hpp" 
#include "gateways/FIXDefinition.hpp"
#include "orderbook/IExecutionListener.hpp"
#include <memory>

class TCPSession; // Forward declaration

class ExchangeOrchestrator : public IExecutionListener {
public:
    // Constructor accepts which protocol gateway we want to use
    ExchangeOrchestrator(std::unique_ptr<IGateway> protocolGateway)
        : gateway_(std::move(protocolGateway)), riskManager_(), orderBook_() {}

    // public-facing methods from the gateway to handle incoming raw wire messages
    std::optional<Order> on_data_received(std::shared_ptr<TCPSession> session, std::string_view raw_data); 

    // callback hooks
    void on_client_connect(std::shared_ptr<TCPSession> session);
    void on_client_disconnect(std::shared_ptr<TCPSession> session);
    void on_order_accepted(const Order& order);
    void on_order_executed(const Order& order, int price, int quantity) override;

    // Output the current state of the order book in console (I/O blocking)
    void outputOrderBookState() const;

private:
    void processOrder(std::shared_ptr<TCPSession> session, Order& order);

    std::shared_ptr<TCPSession> currentSession_ = nullptr; // current session for the incoming order

    // objects for the core components of the exchange
    std::unique_ptr<IGateway> gateway_; // Abstract boundary for protocol decoding
    RiskManager riskManager_; 
    OrderBook orderBook_; 
};