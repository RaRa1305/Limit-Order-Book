#pragma once
#include <cstdint>

enum class OrderType : uint8_t
{
    Buy,
    Sell
};

enum class TimeInForce : uint8_t
{
    GTC,
    IOC,
    FOK
};

enum class OrderCategory : uint8_t
{
    Limit,
    Market
};

struct Order
{
    uint64_t ID;
    int64_t Price;
    uint64_t Quantity;
    OrderType Ordertype;
    TimeInForce TIF;
    OrderCategory Category;
};