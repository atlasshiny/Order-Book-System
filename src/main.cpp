#include <iostream>
#include <vector>
#include <chrono>
#include <string> // Explicitly include string
#include "orderbook/Order.hpp"
#include "orderbook/OrderBook.hpp"
#include "risk/SimpleRiskManager.hpp"

int main() {
    OrderBook orderBook;
    SimpleRiskManager riskManager;

    while (true) {
        int id, price, quantity;
        uint64_t current_time;
        std::string orderTypeStr, directionStr; // Clean separation

        std::cout << "\nEnter (LIMIT/MARKET) (BUY/SELL) Price Qty: ";
        if (!(std::cin >> orderTypeStr >> directionStr >> price >> quantity)) {
            break; // Safely handle stream disruptions
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

        // Get current time in nanoseconds for time priority
        current_time = std::chrono::duration_cast<std::chrono::nanoseconds>(
            std::chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
        
        Order newOrder{orderType, direction, price, quantity, current_time};

        // Risk check before placing the order
        if (riskManager.checkOrder(newOrder) == false) {
            std::cout << "Order rejected by risk manager." << std::endl;
            continue; // Skip placing the order if it fails risk checks
        }
    
        // Add the order to the order book
        if (orderType == OrderType::LIMIT) {
            orderBook.placeLimitOrder(newOrder);
            std::cout << "Limit order placed successfully." << std::endl;
        } else if (orderType == OrderType::MARKET) {
            orderBook.placeMarketOrder(newOrder);
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