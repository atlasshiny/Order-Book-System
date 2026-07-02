#pragma once

struct Order; // Forward declaration

class IExecutionListener {
public:
    virtual ~IExecutionListener() = default;

    // This method is called when an order is executed (fully or partially filled)
    virtual void on_order_executed(const Order& order, int price, int quantity) = 0;
};