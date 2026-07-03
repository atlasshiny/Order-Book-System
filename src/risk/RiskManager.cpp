#include <iostream>
#include "risk/RiskManager.hpp"

bool RiskManager::checkOrder(const Order& order) {
    // Implement basic risk checks here

    if (order.quantity > MAX_ORDER_QUANTITY) {
        std::cout << "Risk Check Failed: Order quantity exceeds maximum allowed." << std::endl;
        return false; // Reject the order
    }

    if (order.quantity <= 0) {
        std::cout << "Risk Check Failed: Order quantity must be positive or non-zero." << std::endl;
        return false; // Reject the order
    }

    if (order.price <= 0) {
        std::cout << "Risk Check Failed: Order price must be positive or non-zero." << std::endl;
        return false; // Reject the order
    }

    if (order.type == OrderType::NONE) {
        std::cout << "Risk Check Failed: Order type is NONE, which is invalid." << std::endl;
        return false; // Reject the order
    }

    if (order.direction != OrderDirection::BUY && order.direction != OrderDirection::SELL) {
        std::cout << "Risk Check Failed: Order direction is invalid." << std::endl;
        return false; // Reject the order
    }

    return true; // Accept the order if all checks pass
}