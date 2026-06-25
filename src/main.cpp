#include <iostream>
#include <cassert>
#include "../include/OrderBook.h"

// GTC partial fill
void test_gtc()
{
    OrderBook book;
    book.add_order({1, 10, 100, OrderType::Sell, TimeInForce::GTC, OrderCategory::Limit});

    book.add_order({2, 10, 40, OrderType::Buy, TimeInForce::GTC, OrderCategory::Limit});

    assert(!book.has_order(2));
    assert(book.has_order(1));
    assert(book.get_order(1).Quantity == 60);
    std::cout << "GTC partial fill passed" << std::endl;
}

// IOC partial fill
void test_ioc()
{
    OrderBook book;
    book.add_order({1, 10, 50, OrderType::Sell, TimeInForce::GTC, OrderCategory::Limit});

    book.add_order({2, 10, 100, OrderType::Buy, TimeInForce::IOC, OrderCategory::Limit}); // Buy the 50, and CANCEL the remaining 50.

    assert(!book.has_order(1));
    assert(!book.has_order(2));
    std::cout << "IOC partial fill passed" << std::endl;
}

// FOK success
void test_fok_success()
{
    OrderBook book;
    book.add_order({1, 10, 50, OrderType::Sell, TimeInForce::GTC, OrderCategory::Limit});
    book.add_order({2, 11, 50, OrderType::Sell, TimeInForce::GTC, OrderCategory::Limit});

    book.add_order({3, 11, 100, OrderType::Buy, TimeInForce::FOK, OrderCategory::Limit});

    assert(!book.has_order(1));
    assert(!book.has_order(2));
    assert(!book.has_order(3));
    std::cout << "FOK success passed" << std::endl;
}

// FOK failure
void test_fok_failure()
{
    OrderBook book;
    book.add_order({1, 10, 80, OrderType::Sell, TimeInForce::GTC, OrderCategory::Limit});

    book.add_order({2, 10, 100, OrderType::Buy, TimeInForce::FOK, OrderCategory::Limit}); // Cant be filled

    assert(book.has_order(1));
    assert(book.get_order(1).Quantity == 80);
    assert(!book.has_order(2));
    std::cout << "FOK failure passed" << std::endl;
}

// Market order
void test_market_order()
{
    OrderBook book;
    book.add_order({1, 500, 100, OrderType::Sell, TimeInForce::GTC, OrderCategory::Limit});

    book.add_order({2, 0, 100, OrderType::Buy, TimeInForce::IOC, OrderCategory::Market}); // Price 0 but orderbook should map it to 1e9

    assert(!book.has_order(1));
    assert(!book.has_order(2));
    std::cout << "Market order passed" << std::endl;
}

int main()
{
    test_gtc();
    test_ioc();
    test_fok_success();
    test_fok_failure();
    test_market_order();

    std::cout << "All tests passed" << std::endl;
    return 0;
}