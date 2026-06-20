#include <iostream>
#include <string>
#include <chrono>
#include <optional>
#include <string_view>

// Include your domain models and protocol components
#include "orderbook/Order.hpp" // Ensure your path matches your Order/OrderType definitions
#include "gateways/FIXDefinition.hpp"
#include "parsers/FIXParser.hpp"
#include "writers/FIXWriter.hpp"
#include "orderbook/OrderBook.hpp"
#include "risk/SimpleRiskManager.hpp"

// Assuming your OrderBook and RiskManager headers are structured like this:
// #include "engine/OrderBook.hpp"
// #include "risk/SimpleRiskManager.hpp"

int main() {
    OrderBook orderBook;
    SimpleRiskManager riskManager;
    
    // Instantiate your new FIX protocol components
    FIXWriter fixWriter;
    FIXParser fixParser;

    // Fixed-size stack allocation for your high-performance write buffer
    char wireBuffer[1024];

    while (true) {
        int price, quantity;
        uint64_t current_time;
        std::string orderTypeStr, directionStr;

        std::cout << "\nEnter (LIMIT/MARKET) (BUY/SELL) Price Qty (or Ctrl+C to exit): ";
        if (!(std::cin >> orderTypeStr >> directionStr >> price >> quantity)) {
            break; 
        }

        // Determine order type
        OrderType orderType;
        if (orderTypeStr == "LIMIT") {
            orderType = OrderType::LIMIT;
        } else if (orderTypeStr == "MARKET") {
            orderType = OrderType::MARKET;
        } else {
            std::cout << "Invalid order type. Please enter LIMIT or MARKET." << std::endl;
            continue;
        }

        // Determine order direction
        OrderDirection direction;
        if (directionStr == "BUY") {
            direction = OrderDirection::BUY;
        } else if (directionStr == "SELL") {
            direction = OrderDirection::SELL;
        } else {
            std::cout << "Invalid direction. Please enter BUY or SELL." << std::endl;
            continue;
        }

        current_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
        
        // Create our local order template representing a client message
        Order consoleOrder{orderType, direction, price, quantity, current_time};

        // STEP 1: FIX WRITER SERIALIZATION (Client Sending Order)
        size_t bytesWritten = fixWriter.write(consoleOrder, wireBuffer, sizeof(wireBuffer));
        
        if (bytesWritten == 0) {
            std::cout << "Error: FIXWriter failed to serialize the message (buffer too small)." << std::endl;
            continue;
        }

        // Create a string view of the raw wire data
        std::string_view rawWireMsg(wireBuffer, bytesWritten);

        // Print the raw wire message to console (replacing SOH with '|' for visibility)
        std::cout << "--- [RAW FIX WIRE MESSAGE SENDING] ---" << std::endl;
        for (char c : rawWireMsg) {
            if (c == '\x01') std::cout << '|';
            else std::cout << c;
        }
        std::cout << "\n--------------------------------------" << std::endl;

        // STEP 2: FIX PARSER DESERIALIZATION (Exchange Gateway Receiving Order)
        std::optional<Order> parsedOrderOpt = fixParser.parse(rawWireMsg);

        if (!parsedOrderOpt.has_value()) {
            std::cout << "Gateway Error: FIXParser rejected or failed to parse the wire message." << std::endl;
            continue;
        }

        // Extract our cleanly deserialized engine order
        Order engineOrder = parsedOrderOpt.value();
        
        // Keep the original console input's side/direction since your current 
        // FIXParser doesn't evaluate Tag 54 (Side) yet.
        engineOrder.direction = consoleOrder.direction; 

        // STEP 3: MATCHING ENGINE EXECUTION
        // Risk check before placing the order
        if (riskManager.checkOrder(engineOrder) == false) {
            std::cout << "Order rejected by risk manager." << std::endl;
            continue; 
        }
    
        // Add the order to the order book
        if (engineOrder.type == OrderType::LIMIT) {
            orderBook.placeLimitOrder(engineOrder);
            std::cout << "Limit order parsed and placed successfully." << std::endl;
        } else if (engineOrder.type == OrderType::MARKET) {
            orderBook.placeMarketOrder(engineOrder);
            std::cout << "Market order parsed and executed successfully." << std::endl;
        }

        // Match existing limit orders in the book
        orderBook.matchOrders();

        // Display Level 1 and Level 2 data after each order
        std::cout << "\nLevel 1 Data:" << std::endl;
        orderBook.level1Data();

        std::cout << "\nLevel 2 Data:" << std::endl;
        orderBook.level2Data();
    }
    return 0;
}