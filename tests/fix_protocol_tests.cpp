#include <gtest/gtest.h>
#include "gateways/FIXGateway.hpp"
#include "writers/FIXWriter.hpp"
#include "parsers/FIXParser.hpp"
#include "orderbook/Order.hpp"
#include <string>

TEST(FIXWriterTest, WriteOrder) {
    FIXWriter writer;
    Order order{OrderType::LIMIT, OrderDirection::BUY, 100, 10, 10, 1234567890, 1, OrderStatus::NEW, 0};
    char buffer[1024];
    size_t bytesWritten = writer.write(order, buffer, sizeof(buffer));

    EXPECT_GT(bytesWritten, 0);
    std::string output(buffer, bytesWritten);

    EXPECT_NE(output.find("35=D"), std::string::npos);
    EXPECT_NE(output.find("54=1"), std::string::npos);
    EXPECT_NE(output.find("38=10"), std::string::npos);  // Corrected: quantity is 10
    EXPECT_NE(output.find("44=100"), std::string::npos); // Corrected: price is 100
    EXPECT_NE(output.find("11=1"), std::string::npos);
}

TEST(FIXParserTest, ParseOrder) {
    FIXParser parser;
    // Provide an integer timestamp field (Tag 60) so validation doesn't reject it for being 0 (with a value of 1685622600000)
    std::string fixMessage = "8=FIX.4.2|9=176|35=D|49=CLIENT1|56=SERVER1|34=1|11=12345|21=1|55=AAPL|54=1|38=100|40=2|44=150|60=1685622600000|10=128|";

    // Replace '|' with SOH character for actual parsing
    for (char& c : fixMessage) {
        if (c == '|') c = '\x01';
    }

    auto orderOpt = parser.parse(fixMessage);

    ASSERT_TRUE(orderOpt.has_value());
    Order order = orderOpt.value();

    EXPECT_EQ(order.type, OrderType::LIMIT);
    EXPECT_EQ(order.direction, OrderDirection::BUY);
    EXPECT_EQ(order.price, 150);
    EXPECT_EQ(order.quantity, 100);
}