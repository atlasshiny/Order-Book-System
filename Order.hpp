#pragma once
#include <array>

// all special "types" needed for an order
enum class OrderType {
    BUY,
    SELL,
    HOLD
};

// the order itself, a structure holding all relevant information
// and the main object that the program will interact with.
struct Order {
    OrderType type;
    int id;
    int price;
    int quantity;
};