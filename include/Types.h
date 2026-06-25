
#include<cstdint>

enum class OrderType
{
    Buy,
    Sell
};

struct Order
{
    uint64_t ID;
    int64_t Price;
    uint64_t Quantity;
    OrderType Ordertype;
};