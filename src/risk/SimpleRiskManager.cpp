#include <iostream>
#include "risk/SimpleRiskManager.hpp"

bool SimpleRiskManager::checkOrder(const Order& order) {
    // Implement basic risk checks here

    if (order.quantity > MAX_ORDER_QUANTITY) {
        std::cout << "Risk Check Failed: Order quantity exceeds maximum allowed." << std::endl;
        return false; // Reject the order
    }

    if (order.quantity <= 0) {
        std::cout << "Risk Check Failed: Order quantity must be positive." << std::endl;
        return false; // Reject the order
    }

    if (order.price <= 0) {
        std::cout << "Risk Check Failed: Order price must be positive." << std::endl;
        return false; // Reject the order
    }

    return true; // Accept the order if all checks pass
}