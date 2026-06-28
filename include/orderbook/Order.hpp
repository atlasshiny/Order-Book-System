#pragma once
#include <array>
#include <cstdint>

// all special "types" needed for an order
// order direction (BUY/SELL)
enum class OrderDirection {
    BUY,
    SELL
};

// order status (OPEN, PARTIALLY_FILLED, FILLED, CANCELLED)
enum class OrderStatus {
    OPEN,
    PARTIALLY_FILLED,
    FILLED,
    CANCELLED
};

// order type (LIMIT, MARKET, IOC, POST_ONLY)
enum class OrderType {
    NONE = 0,
    LIMIT,
    MARKET,
    IOC,
    POST_ONLY
};

// the order itself, a structure holding all relevant information
// and the main object that the program will interact with.
struct Order {
    OrderType type;
    OrderDirection direction;
    int price;
    int quantity;
    uint64_t timestamp; // Time in nanoseconds since order creation, used for time priority in matching
    int clientID;
    OrderStatus status = OrderStatus::OPEN; // Default status is OPEN
    int id; // Unique order ID assigned by the matching engine
};