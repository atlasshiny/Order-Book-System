#include <iostream>
#include <vector>
#include <chrono>
#include <string> // Explicitly include string
#include "Order.hpp"
#include "OrderBook.hpp"

using namespace std;

int main() {
    OrderBook orderBook;

    while (true) {
        int id, price, quantity;
        uint64_t current_time;
        string orderTypeStr, directionStr; // Clean separation
        
        cout << "\nEnter (LIMIT/MARKET) (BUY/SELL) ID Price Qty: ";
        if (!(cin >> orderTypeStr >> directionStr >> id >> price >> quantity)) {
            break; // Safely handle stream disruptions
        }

        // Determine order type
        OrderType orderType;
        if (orderTypeStr == "LIMIT") {
            orderType = OrderType::LIMIT;
        } else if (orderTypeStr == "MARKET") {
            orderType = OrderType::MARKET;
        } else {
            cout << "Invalid order type. Please enter LIMIT or MARKET." << endl;
            continue;
        }

        // Determine order direction
        OrderDirection direction;
        if (directionStr == "BUY") {
            direction = OrderDirection::BUY;
        } else if (directionStr == "SELL") {
            direction = OrderDirection::SELL;
        } else {
            cout << "Invalid direction. Please enter BUY or SELL." << endl;
            continue;
        }

        // Get current time in nanoseconds for time priority
        current_time = chrono::duration_cast<chrono::nanoseconds>(
            chrono::high_resolution_clock::now().time_since_epoch()
        ).count();
        
        Order newOrder{orderType, direction, id, price, quantity, current_time};

        // Add the order to the order book
        if (orderType == OrderType::LIMIT) {
            orderBook.placeLimitOrder(newOrder);
            cout << "Limit order placed successfully." << endl;
        } else if (orderType == OrderType::MARKET) {
            orderBook.placeMarketOrder(newOrder);
        }

        // Match existing limit orders in the book
        orderBook.matchOrders();
    }
    return 0;
}