#include <gateways/FIXGateway.hpp>
#include <parsers/FIXParser.hpp>
#include <writers/FIXWriter.hpp>
#include <iostream>

// Serialize and send an order to the exchange (assumes the order is already built and validated)
size_t FIXGateway::sendOrder(const Order& order, char* wireBuffer_, size_t bufferSize) {
    // Serialize the order into FIX format
    size_t bytesWritten = fixWriter_.write(order, wireBuffer_, bufferSize);
        
    if (bytesWritten == 0) {
        std::cerr << "Error: FIXWriter failed to serialize the message (buffer too small)." << std::endl;
        return 0;
    }

    // Here you would typically send the wireBuffer_ over a network socket
    std::cout << "Sending FIX message: ";
    for (size_t i = 0; i < bytesWritten; ++i) {
        if (wireBuffer_[i] == '\x01') std::cout << '|';
        else std::cout << wireBuffer_[i];
    }
    std::cout << std::endl;

    return bytesWritten; // Return the number of bytes written for confirmation
}

// Receive and parse an incoming FIX message from the exchange
std::optional<Order> FIXGateway::receiveOrder(std::string_view rawData) {
    // Parse the incoming FIX message
    auto orderOpt = fixParser_.parse(rawData);
        
    if (!orderOpt) {
        std::cerr << "Error: FIXParser failed to parse the incoming message." << std::endl;
        return std::nullopt;
    }

    const Order& order = *orderOpt;
    // Here you would typically process the order (e.g., add it to the order book)
    std::cout << "Received Order - Type: " << (order.type == OrderType::LIMIT ? "LIMIT" : "MARKET")
                << ", Direction: " << (order.direction == OrderDirection::BUY ? "BUY" : "SELL")
                << ", Price: " << order.price
                << ", Quantity: " << order.quantity
                << ", Timestamp: " << order.timestamp
                << ", Status: " << (order.status == OrderStatus::OPEN ? "OPEN" : "CLOSED")
                << ", ID: " << order.id
                << std::endl;
    return orderOpt;
}

void FIXGateway::cancelOrder(int orderId) {
    // Logic to cancel an order by ID (not implemented yet)
    std::cout << "Canceling order with ID: " << orderId << std::endl;
}

void FIXGateway::onMarketDataUpdate() {
    // Logic to handle market data updates (not implemented yet)
    std::cout << "Market data update received." << std::endl;
}