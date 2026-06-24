#include <cstdint>

enum class OrderType
{
    Buy,
    Sell
};

struct Order
{
    std::uint64_t ID;
    std::int64_t Price;
    std::uint64_t Quantity;
    OrderType Ordertype;
};

int main()
{
    return 0;
}