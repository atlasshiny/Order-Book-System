#include <gtest/gtest.h>
#include "risk/RiskManager.hpp"

TEST(RiskManagerTest, OrderValidation) {
    RiskManager riskManager;
    
    // Test a valid order
    Order validOrder{OrderType::LIMIT, OrderDirection::BUY, 100, 10, 10, 1234567890, 1, OrderStatus::NEW, 0};
    EXPECT_TRUE(riskManager.checkOrder(validOrder));
}

TEST(RiskManagerTest, ZeroQuantityOrder) {
    RiskManager riskManager;
    // Test an order with zero quantity
    Order zeroQuantityOrder{OrderType::LIMIT, OrderDirection::BUY, 100, 0, 0, 1234567890, 1, OrderStatus::NEW, 0};
    EXPECT_FALSE(riskManager.checkOrder(zeroQuantityOrder));
}

TEST(RiskManagerTest, NegativePriceOrder) {
    RiskManager riskManager;
    // Test an order with negative price
    Order negativePriceOrder{OrderType::LIMIT, OrderDirection::BUY, -100, 10, 10, 1234567890, 1, OrderStatus::NEW, 0};
    EXPECT_FALSE(riskManager.checkOrder(negativePriceOrder));
    
}

TEST(RiskManagerTest, InvalidOrderType) {
    RiskManager riskManager;
    // Test an order with invalid type
    Order invalidTypeOrder{OrderType::NONE, OrderDirection::BUY, 100, 10, 10, 1234567890, 1, OrderStatus::NEW, 0};
    EXPECT_FALSE(riskManager.checkOrder(invalidTypeOrder));
}

TEST(RiskManagerTest, InvalidOrderDirection) {
    RiskManager riskManager;
    // Test an order with invalid direction
    Order invalidDirectionOrder{OrderType::LIMIT, static_cast<OrderDirection>(-1), 100, 10, 10, 1234567890, 1, OrderStatus::NEW, 0};
    EXPECT_FALSE(riskManager.checkOrder(invalidDirectionOrder));
}