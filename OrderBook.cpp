#include <algorithm>
#include <iostream>
#include "OrderBook.hpp"

using namespace std;

void OrderBook::addOrder(const Order& order) {
    if (order.type == OrderType::BUY) {
        bidOrders.push_back(order);
    } else if (order.type == OrderType::SELL) {
        askOrders.push_back(order);
    }
}

void OrderBook::matchOrders() {
    // sort best bids first (highest price) and best asks first (lowest price)
    sort(bidOrders.begin(), bidOrders.end(), [](const Order& a, const Order& b) {
        return a.price > b.price;
    });
    sort(askOrders.begin(), askOrders.end(), [](const Order& a, const Order& b) {
        return a.price < b.price;
    });

    // match while there is an overlap between highest bid and lowest ask
    while (!bidOrders.empty() && !askOrders.empty() && bidOrders.front().price >= askOrders.front().price) {
        Order& buy = bidOrders.front();
        Order& sell = askOrders.front();

        // determine trade quantity (fill as much as possible)
        int tradeQuantity = std::min(buy.quantity, sell.quantity);

        // determine trade price (use midpoint)
        int tradePrice = (buy.price + sell.price) / 2;

        cout << "Matched BUY id " << buy.id
             << " (price " << buy.price << ", qty " << tradeQuantity << ") with SELL id " << sell.id
             << " (price " << sell.price << ", qty " << tradeQuantity << ") at trade price " << tradePrice << endl;

        // deduct executed quantity from both orders
        buy.quantity -= tradeQuantity;
        sell.quantity -= tradeQuantity;

        // remove fully filled orders
        if (buy.quantity <= 0) {
            bidOrders.erase(bidOrders.begin());
        }
        if (sell.quantity <= 0) {
            askOrders.erase(askOrders.begin());
        }
    }
}