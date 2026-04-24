#include <iostream>
#include <vector>
#include "Order.hpp"
#include "OrderBook.hpp"

using namespace std;

void addHistory(Order& order, vector<Order>& history);

int main() {
    //create an order book object
    OrderBook orderBook;

    while (true) {
        //get user input for order details
        int id, price, quantity;
        string type;
        cout << "Enter order type (BUY/SELL/HOLD), id, price, and quantity: ";
        cin >> type >> id >> price >> quantity;

        //create an order object based on user input
        OrderType orderType;
        if (type == "BUY") {
            orderType = OrderType::BUY;
        } else if (type == "SELL") {
            orderType = OrderType::SELL;
        } else {
            orderType = OrderType::HOLD;
        }
        Order newOrder{orderType, id, price};

        //add the order to the order book
        orderBook.addOrder(newOrder);

        //match orders in the order book
        orderBook.matchOrders();
    }
}

void addHistory(Order& order, vector<Order>& history) {
    history.push_back(order);
}