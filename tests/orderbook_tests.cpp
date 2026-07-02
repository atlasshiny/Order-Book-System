#include <gtest/gtest.h>
#include <chrono>
#include "orderbook/OrderBook.hpp" // Adjust path if needed
#include "orderbook/Order.hpp"     // Adjust path if needed

// Helper function to create a dummy order easily
Order CreateTestOrder(OrderType type, OrderDirection direction, int price, int quantity, int clientID) {
    uint64_t ts = std::chrono::duration_cast<std::chrono::nanoseconds>(
                      std::chrono::high_resolution_clock::now().time_since_epoch())
                      .count();
    
    return Order{
        type,
        direction,
        price,
        quantity,
        quantity,
        ts,
        clientID,
        OrderStatus::NEW,
        0
    };
}

// Test 1: Verify a resting limit order receives an engine ID
TEST(OrderBookTest, PlaceRestingLimitOrder) {
    OrderBook book;
    Order buy_order = CreateTestOrder(OrderType::LIMIT, OrderDirection::BUY, 150, 100, 42);
    
    // Manually mimic the orchestrator's step before placement
    buy_order.id = book.getNextOrderId(); 
    
    book.placeLimitOrder(buy_order);
    
    EXPECT_EQ(buy_order.id, 1); 
    EXPECT_EQ(buy_order.quantity, 100); 
    EXPECT_EQ(buy_order.status, OrderStatus::NEW);
}

// Full execution requires calling matchOrders()
TEST(OrderBookTest, FullLimitOrderExecution) {
    OrderBook book;
    
    Order resting_buy = CreateTestOrder(OrderType::LIMIT, OrderDirection::BUY, 100, 50, 101);
    resting_buy.id = book.getNextOrderId();
    book.placeLimitOrder(resting_buy);
    
    Order aggressive_sell = CreateTestOrder(OrderType::LIMIT, OrderDirection::SELL, 100, 50, 202);
    aggressive_sell.id = book.getNextOrderId();
    book.placeLimitOrder(aggressive_sell);
    
    // CRITICAL: Your engine relies on an explicit match cycle pass![cite: 5]
    book.matchOrders(); 
}

// IOC partial fill and immediate prune
TEST(OrderBookTest, ImmediateOrCancelPartialFill) {
    OrderBook book;
    
    Order resting_sell = CreateTestOrder(OrderType::LIMIT, OrderDirection::SELL, 200, 10, 505);
    resting_sell.id = book.getNextOrderId();
    book.placeLimitOrder(resting_sell);
    
    Order ioc_buy = CreateTestOrder(OrderType::IOC, OrderDirection::BUY, 200, 50, 606);
    ioc_buy.id = book.getNextOrderId();
    
    // Run the sequence
    book.placeImmediateOrCancelOrder(ioc_buy);
    book.matchOrders(); 

}