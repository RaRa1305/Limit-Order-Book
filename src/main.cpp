#include <iostream>
#include "../include/OrderBook.h"

int main() {
    OrderBook book;
    book.add_order({1, 100, 50, OrderType::Sell});
    std::cout << "OK" << std::endl;
    return 0;
}