#include <cstdint>
#include <unordered_map>
#include <map>
#include <deque>
#include <vector>
#include <algorithm>

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

class OrderBook
{
private:
    std::unordered_map<uint64_t, Order> OrderList;
    std::map<int64_t, std::deque<uint64_t>, std::greater<int64_t>> Bids;
    std::map < int64_t, std::deque<uint64_t>> Asks;
};

int main()
{
    return 0;
}