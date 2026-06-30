#include "orchestrator/ExchangeOrchestrator.hpp"
#include "network/TCPSession.hpp"
#include <iostream>

void ExchangeOrchestrator::processOrder(std::shared_ptr<TCPSession> session, Order& order) {
    // set the current session for the incoming order
    currentSession_ = session;
    orderBook_.set_orchestrator(this); // set the orchestrator for callback hooks
    
    // Run high-speed concrete pre-trade risk checks
    if (!riskManager_.checkOrder(order)) {
        std::cout << "Order rejected by risk manager." << std::endl; // I/O blocking
        return; 
    }
    
    order.originalQuantity = order.quantity; // Store the original quantity for reference

    on_order_accepted(order); // Notify the client that the order has been accepted

    // Process inside the concrete matching engine
    if (order.type == OrderType::LIMIT) {
        orderBook_.placeLimitOrder(order);
        std::cout << "Limit order parsed and placed successfully." << std::endl; // I/O blocking
    } else {
        orderBook_.placeMarketOrder(order);
        std::cout << "Market order parsed and executed successfully." << std::endl; // I/O blocking
    }

    orderBook_.matchOrders();

    // Output the current state of the order book for debugging purposes
    outputOrderBookState(); // I/O blocking

    // Clear out the pointers when this single transaction loop finishes
    orderBook_.remove_orchestrator();
    currentSession_ = nullptr;
};

std::optional<Order> ExchangeOrchestrator::on_data_received(std::shared_ptr<TCPSession> session, std::string_view raw_data) {
    std::optional<Order> order = gateway_->on_data_received(session, raw_data);

    if (order) {
        processOrder(session, order.value());
    }

    return order;
}

void ExchangeOrchestrator::on_client_connect(std::shared_ptr<TCPSession> session) {
    gateway_->on_client_connect(session);
}

void ExchangeOrchestrator::on_client_disconnect(std::shared_ptr<TCPSession> session) {
    gateway_->on_client_disconnect(session);
}

void ExchangeOrchestrator::on_order_accepted(const Order& order) {
    if(!currentSession_) {
        return; // return if no session is set (like in console mode)
    }
    // create string_views for ExecType and OrdStatus
    std::string_view execTypeView(&FIX::ExecTypes::New, 1);
    std::string_view ordStatusView(&FIX::OrdStatuses::New, 1);

    size_t bytes = gateway_->get_writer().writeExecutionReport(
        order, execTypeView, ordStatusView, currentSession_->get_buffer_ptr(), currentSession_->get_buffer_size()
    );

    if (bytes > 0) {
        currentSession_->write(std::string(currentSession_->get_buffer_ptr(), bytes));
    }
}

void ExchangeOrchestrator::on_order_executed(const Order& order, int price, int quantity) {
    if(!currentSession_) {
        return; // return if no session is set (like in console mode)
    }

    char currentStatus;

    if (order.quantity == 0) {
        currentStatus = FIX::OrdStatuses::Filled;
    } else {
        currentStatus = FIX::OrdStatuses::PartiallyFilled;
    }

    std::string_view execTypeView(&FIX::ExecTypes::Trade, 1);
    std::string_view ordStatusView(&currentStatus, 1);

    size_t bytes = gateway_->get_writer().writeExecutionReport(
        order, execTypeView, ordStatusView, currentSession_->get_buffer_ptr(), currentSession_->get_buffer_size()
    );

    if (bytes > 0) {
        currentSession_->write(std::string(currentSession_->get_buffer_ptr(), bytes));
    }
}

void ExchangeOrchestrator::outputOrderBookState() const {
        std::cout << "\nLevel 1 Data:" << std::endl;
        orderBook_.level1Data();

        std::cout << "\nLevel 2 Data:" << std::endl;
        orderBook_.level2Data();
};