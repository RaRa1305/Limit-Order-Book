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
    uint64_t ID = 0;
    int64_t Price = 0;
    uint64_t Quantity = 0;
    OrderType Ordertype = OrderType::Buy;
    TimeInForce TIF = TimeInForce::GTC;
    OrderCategory Category = OrderCategory::Limit;
};