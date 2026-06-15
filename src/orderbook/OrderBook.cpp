#include <algorithm>
#include <iostream>
#include <map>
#include <iomanip>
#include "orderbook/OrderBook.hpp"

using namespace std;

void OrderBook::matchAgainstBook(Order& incomingOrder, std::deque<Order>& oppositeBook) {
    // 1. Ensure the book is sorted by price/time priority before matching
    // (You can either call your sort logic here or ensure it's sorted before calling this)
    
    while (!oppositeBook.empty() && incomingOrder.quantity > 0) {
        Order& restingOrder = oppositeBook.front();

        // 2. Determine price limit constraints
        // If it's a BUY order, it can only match if the resting SELL price <= incoming BUY price
        // If it's a SELL order, it can only match if the resting BUY price >= incoming SELL price
        if (incomingOrder.direction == OrderDirection::BUY && incomingOrder.price < restingOrder.price) break;
        if (incomingOrder.direction == OrderDirection::SELL && incomingOrder.price > restingOrder.price) break;

        // 3. Match quantities
        int tradeQuantity = std::min(incomingOrder.quantity, restingOrder.quantity);
        
        // 4. Update quantities
        incomingOrder.quantity -= tradeQuantity;
        restingOrder.quantity -= tradeQuantity;

        cout << "Trade Executed: " << tradeQuantity << " shares at $" << restingOrder.price << endl;

        // 5. Cleanup filled orders
        if (restingOrder.quantity <= 0) {
            restingOrder.status = OrderStatus::FILLED;
            oppositeBook.pop_front();
        }
    }
}

void OrderBook::placeLimitOrder(Order& order) {
    order.id = nextOrderId++; // Assign a unique ID to the order

    if (order.direction == OrderDirection::BUY) {
        // Add limit buy order to bid side
        bidOrders.push_back(order);
    } else if (order.direction == OrderDirection::SELL) {
        // Add limit sell order to ask side
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

        matchAgainstBook(order, askOrders);

        // Post-execution report
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

        matchAgainstBook(order, bidOrders);

        // Post-execution report
        if (order.quantity > 0) {
            cout << "Market SELL ID " << order.id 
                 << " expired unfilled for " << order.quantity 
                 << " shares due to total lack of market liquidity." << endl;
        }
    }
}

void OrderBook::placeImmediateOrCancelOrder(Order& order) {
    // Implementation for Immediate-Or-Cancel (IOC) orders
    // This function will attempt to fill the order immediately and cancel any unfilled portion
    // Similar logic to placeMarketOrder but with cancellation of remaining quantity

    order.id = nextOrderId++; // Assign a unique ID to the order

    if (order.direction == OrderDirection::BUY) {
        if (order.quantity > 0) {
            // ensure best asks are sorted first (lowest price) for market orders
            sort(askOrders.begin(), askOrders.end(), [](const Order& a, const Order& b) {
                if (a.price == b.price) return a.timestamp < b.timestamp;
                return a.price < b.price;
            });

            matchAgainstBook(order, askOrders);

            if (order.quantity > 0) {
                cout << "IOC BUY ID " << order.id 
                     << " cancelled for remaining " << order.quantity 
                     << " shares that could not be filled immediately." << endl;
            }
        } else {
            cout << "IOC BUY ID " << order.id 
                 << " cancelled immediately as it had zero quantity." << endl;
        }
    } else if (order.direction == OrderDirection::SELL) {
        if (order.quantity > 0) {
            // Ensure the bid book is sorted highest-price-first before sweeping
            sort(bidOrders.begin(), bidOrders.end(), [](const Order& a, const Order& b) {
                if (a.price == b.price) return a.timestamp < b.timestamp;
                return a.price > b.price;
            });

            matchAgainstBook(order, bidOrders);

            if (order.quantity > 0) {
                cout << "IOC SELL ID " << order.id 
                     << " cancelled for remaining " << order.quantity 
                     << " shares that could not be filled immediately." << endl;
            }
        } else {
            cout << "IOC SELL ID " << order.id 
                 << " cancelled immediately as it had zero quantity." << endl;
        }
    }
}

void OrderBook::placePostOnlyOrder(Order& order) {
    // Implementation for Post-Only orders
    // This function will only add the order to the book if it does not immediately match
    // If it would match, it should be rejected or cancelled

    order.id = nextOrderId++; // Assign a unique ID to the order

    if (order.direction == OrderDirection::BUY) {
        if (!askOrders.empty() && order.price >= askOrders.front().price) {
            cout << "Post-Only BUY ID " << order.id 
                 << " rejected as it would immediately match with existing SELL orders." << endl;
            return; // Do not add to book
        }
        // Add limit buy order to bid side
        bidOrders.push_back(order);
    } else if (order.direction == OrderDirection::SELL) {
        if (!bidOrders.empty() && order.price <= bidOrders.front().price) {
            cout << "Post-Only SELL ID " << order.id 
                 << " rejected as it would immediately match with existing BUY orders." << endl;
            return; // Do not add to book
        }
        // Add limit sell order to ask side
        askOrders.push_back(order);
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