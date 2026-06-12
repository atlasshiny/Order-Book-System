#pragma once
#include <array>
#include <cstdint>

// all special "types" needed for an order
enum class OrderDirection {
    BUY,
    SELL
};

enum class OrderStatus {
    OPEN,
    PARTIALLY_FILLED,
    FILLED,
    CANCELLED
};

enum OrderType {
    LIMIT,
    MARKET
};

// the order itself, a structure holding all relevant information
// and the main object that the program will interact with.
struct Order {
    OrderType type;
    OrderDirection direction;
    int price;
    int quantity;
    uint64_t timestamp; // Time in nanoseconds since order creation, used for time priority in matching
    OrderStatus status = OrderStatus::OPEN; // Default status is OPEN
    int id;
};