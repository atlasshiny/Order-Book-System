#include <algorithm>
#include <iostream>
#include "OrderBook.hpp"

using namespace std;

void OrderBook::placeLimitOrder(const Order& order) {
    if (order.direction == OrderDirection::BUY) {
        // Add limit buy order to bid side
        bidOrders.push_back(order);
    } else if (order.direction == OrderDirection::SELL) {
        askOrders.push_back(order);
    }
}

void OrderBook::placeMarketOrder(const Order& order) {
    Order marketOrder = order; // Make a copy to modify quantity during matching

    if (marketOrder.direction == OrderDirection::BUY) {
        // ensure best asks are sorted first (lowest price) for market orders
        sort(askOrders.begin(), askOrders.end(), [](const Order& a, const Order& b) {
            if (a.price == b.price) return a.timestamp < b.timestamp;
            return a.price < b.price;
        });

        // match market buy order against best available asks
        while (!askOrders.empty() && marketOrder.quantity > 0) {
            Order& bestAsk = askOrders.front(); // Get the cheapest seller available

            // Determine how many shares can cross right now
            int tradeQuantity = std::min(marketOrder.quantity, bestAsk.quantity);
            int tradePrice = bestAsk.price; // Market orders accept the seller's price!

            cout << "Market BUY ID " << marketOrder.id 
                 << " filled " << tradeQuantity << " shares against SELL ID " << bestAsk.id 
                 << " at execution price $" << tradePrice << endl;

            // Deduct the filled quantities
            marketOrder.quantity -= tradeQuantity;
            bestAsk.quantity -= tradeQuantity;

            // If the resting limit order is completely filled, remove it
            if (bestAsk.quantity <= 0) {
                bestAsk.status = OrderStatus::FILLED;
                askOrders.pop_front();
            }
        }

        // 3. Post-execution report
        if (marketOrder.quantity > 0) {
            cout << "Market BUY ID " << marketOrder.id 
                 << " expired unfilled for " << marketOrder.quantity 
                 << " shares due to total lack of market liquidity." << endl;
        }
    } 
    else if (marketOrder.direction == OrderDirection::SELL) {
        // Ensure the bid book is sorted highest-price-first before sweeping
        sort(bidOrders.begin(), bidOrders.end(), [](const Order& a, const Order& b) {
            if (a.price == b.price) return a.timestamp < b.timestamp;
            return a.price > b.price;
        });

        // Sweep the bid book (matching against resting buyers)
        while (!bidOrders.empty() && marketOrder.quantity > 0) {
            Order& bestBid = bidOrders.front(); // Get the highest buyer available

            int tradeQuantity = std::min(marketOrder.quantity, bestBid.quantity);
            int tradePrice = bestBid.price; // Market orders accept the buyer's price!

            cout << "Market SELL ID " << marketOrder.id 
                 << " filled " << tradeQuantity << " shares against BUY ID " << bestBid.id 
                 << " at execution price $" << tradePrice << endl;

            marketOrder.quantity -= tradeQuantity;
            bestBid.quantity -= tradeQuantity;

            if (bestBid.quantity <= 0) {
                bestBid.status = OrderStatus::FILLED;
                bidOrders.pop_front();
            }
        }

        if (marketOrder.quantity > 0) {
            cout << "Market SELL ID " << marketOrder.id 
                 << " expired unfilled for " << marketOrder.quantity 
                 << " shares due to total lack of market liquidity." << endl;
        }
    }
}

void OrderBook::matchOrders() {
    // sort best bids first (highest price) and best asks first (lowest price)
    sort(bidOrders.begin(), bidOrders.end(), [](const Order& a, const Order& b) {
        if (a.price == b.price) {
            return a.timestamp < b.timestamp; // earlier orders have priority
        }
        return a.price > b.price;
    });
    sort(askOrders.begin(), askOrders.end(), [](const Order& a, const Order& b) {
        if (a.price == b.price) {
            return a.timestamp < b.timestamp; // earlier orders have priority
        }
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

        // remove fully filled orders using pop_front() for deques
        if (buy.quantity > 0) {
            buy.status = OrderStatus::PARTIALLY_FILLED;
        } else {
            buy.status = OrderStatus::FILLED;
            bidOrders.pop_front();
        }
        if (sell.quantity > 0) {
            sell.status = OrderStatus::PARTIALLY_FILLED;
        } else {
            sell.status = OrderStatus::FILLED;
            askOrders.pop_front();
        }
    }
}