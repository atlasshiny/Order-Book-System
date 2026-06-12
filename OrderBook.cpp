#include <algorithm>
#include <iostream>
#include <map>
#include <iomanip>
#include "OrderBook.hpp"

using namespace std;

void OrderBook::placeLimitOrder(Order& order) {
    order.id = nextOrderId++; // Assign a unique ID to the order

    if (order.direction == OrderDirection::BUY) {
        // Add limit buy order to bid side
        bidOrders.push_back(order);
    } else if (order.direction == OrderDirection::SELL) {
        askOrders.push_back(order);
    }
}

void OrderBook::placeMarketOrder(Order& order) {
    order.id = nextOrderId++; // Assign a unique ID to the order

    if (order.direction == OrderDirection::BUY) {
        // ensure best asks are sorted first (lowest price) for market orders
        sort(askOrders.begin(), askOrders.end(), [](const Order& a, const Order& b) {
            if (a.price == b.price) return a.timestamp < b.timestamp;
            return a.price < b.price;
        });

        // match market buy order against best available asks
        while (!askOrders.empty() && order.quantity > 0) {
            Order& bestAsk = askOrders.front(); // Get the cheapest seller available

            // Determine how many shares can cross right now
            int tradeQuantity = std::min(order.quantity, bestAsk.quantity);
            int tradePrice = bestAsk.price; // Market orders accept the seller's price!

            cout << "Market BUY ID " << order.id 
                 << " filled " << tradeQuantity << " shares against SELL ID " << bestAsk.id 
                 << " at execution price $" << tradePrice << endl;

            // Deduct the filled quantities
            order.quantity -= tradeQuantity;
            bestAsk.quantity -= tradeQuantity;

            // If the resting limit order is completely filled, remove it
            if (bestAsk.quantity <= 0) {
                bestAsk.status = OrderStatus::FILLED;
                askOrders.pop_front();
            }
        }

        // 3. Post-execution report
        if (order.quantity > 0) {
            cout << "Market BUY ID " << order.id 
                 << " expired unfilled for " << order.quantity 
                 << " shares due to total lack of market liquidity." << endl;
        }
    } 
    else if (order.direction == OrderDirection::SELL) {
        // Ensure the bid book is sorted highest-price-first before sweeping
        sort(bidOrders.begin(), bidOrders.end(), [](const Order& a, const Order& b) {
            if (a.price == b.price) return a.timestamp < b.timestamp;
            return a.price > b.price;
        });

        // Sweep the bid book (matching against resting buyers)
        while (!bidOrders.empty() && order.quantity > 0) {
            Order& bestBid = bidOrders.front(); // Get the highest buyer available

            int tradeQuantity = std::min(order.quantity, bestBid.quantity);
            int tradePrice = bestBid.price; // Market orders accept the buyer's price!

            cout << "Market SELL ID " << order.id 
                 << " filled " << tradeQuantity << " shares against BUY ID " << bestBid.id 
                 << " at execution price $" << tradePrice << endl;

            order.quantity -= tradeQuantity;
            bestBid.quantity -= tradeQuantity;

            if (bestBid.quantity <= 0) {
                bestBid.status = OrderStatus::FILLED;
                bidOrders.pop_front();
            }
        }

        if (order.quantity > 0) {
            cout << "Market SELL ID " << order.id 
                 << " expired unfilled for " << order.quantity 
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

void OrderBook::cancelOrder(int orderId) {
    // Search for the order in bid orders
    auto bidIt = find_if(bidOrders.begin(), bidOrders.end(), [orderId](const Order& o) {
        return o.id == orderId;
    });
    if (bidIt != bidOrders.end()) {
        bidIt->status = OrderStatus::CANCELLED;
        cout << "Cancelled BUY order ID " << orderId << endl;
        bidOrders.erase(bidIt);
        return;
    }

    // Search for the order in ask orders
    auto askIt = find_if(askOrders.begin(), askOrders.end(), [orderId](const Order& o) {
        return o.id == orderId;
    });
    if (askIt != askOrders.end()) {
        askIt->status = OrderStatus::CANCELLED;
        cout << "Cancelled SELL order ID " << orderId << endl;
        askOrders.erase(askIt);
        return;
    }

    cout << "Order ID " << orderId << " not found for cancellation." << endl;
}

void OrderBook::level1Data() const {
    if (!bidOrders.empty()) {
        cout << "Best Bid: $" << bidOrders.front().price << " x " << bidOrders.front().quantity << endl;
    } else {
        cout << "Best Bid: N/A" << endl;
    }

    if (!askOrders.empty()) {
        cout << "Best Ask: $" << askOrders.front().price << " x " << askOrders.front().quantity << endl;
    } else {
        cout << "Best Ask: N/A" << endl;
    }
}

void OrderBook::level2Data() const {
    // 1. Aggregate Bids (Highest Price first)
    std::map<int, int, std::greater<int>> aggregatedBids;
    for (const auto& order : bidOrders) {
        aggregatedBids[order.price] += order.quantity;
    }

    // 2. Aggregate Asks (Lowest Price first)
    std::map<int, int, std::less<int>> aggregatedAsks;
    for (const auto& order : askOrders) {
        aggregatedAsks[order.price] += order.quantity;
    }

    // 3. Render a beautiful Side-by-Side Market Depth Dashboard
    cout << "\n================= LEVEL 2 MARKET DEPTH =================" << endl;
    cout << left << setw(15) << "BID QUANTITY" << setw(12) << "BID PRICE" 
         << " | " 
         << left << setw(12) << "ASK PRICE" << setw(15) << "ASK QUANTITY" << endl;
    cout << "--------------------------------------------------------" << endl;

    auto bidIt = aggregatedBids.begin();
    auto askIt = aggregatedAsks.begin();

    // Loop until both books run out of price depth layers
    while (bidIt != aggregatedBids.end() || askIt != aggregatedAsks.end()) {
        // Print Bid Column
        if (bidIt != aggregatedBids.end()) {
            cout << left << setw(15) << bidIt->second 
                 << "$" << setw(11) << bidIt->first;
            ++bidIt;
        } else {
            cout << left << setw(15) << "" << setw(12) << ""; // Blank pad
        }

        cout << " | ";

        // Print Ask Column
        if (askIt != aggregatedAsks.end()) {
            cout << "$" << left << setw(11) << askIt->first 
                 << setw(15) << askIt->second;
            ++askIt;
        } else {
            cout << left << setw(12) << "" << setw(15) << ""; // Blank pad
        }
        cout << endl;
    }
    cout << "========================================================" << endl;
}