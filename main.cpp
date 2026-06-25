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
    std::map<int64_t, std::deque<uint64_t>> Asks;

public:
    void match_buy(Order &buy_order)
    {
        while (!Asks.empty() && buy_order.Quantity > 0)
        {
            auto best_ask = Asks.begin();
            int64_t best_ask_price = best_ask->first;
            auto &ask_queue = best_ask->second;

            if (buy_order.Price < best_ask_price)
                break;

            else
            {
                int64_t ask_id = ask_queue.front();
                uint64_t fulfill_qty = std::min(OrderList[ask_id].Quantity, buy_order.Quantity);
                OrderList[ask_id].Quantity -= fulfill_qty;
                buy_order.Quantity -= fulfill_qty;

                if (OrderList[ask_id].Quantity == 0)
                    ask_queue.pop_front();
            }

            if (ask_queue.empty())
                Asks.erase(best_ask_price);
        }
    }

    void match_sell(Order &sell_order)
    {
        while (!Bids.empty() && sell_order.Quantity > 0)
        {
            auto best_bid = Bids.begin();
            int64_t best_bid_price = best_bid->first;
            auto &bid_queue = best_bid->second;

            if (sell_order.Price > best_bid_price)
                break;

            else
            {
                int64_t bid_id = bid_queue.front();
                uint64_t fulfill_qty = std::min(OrderList[bid_id].Quantity, sell_order.Quantity);
                OrderList[bid_id].Quantity -= fulfill_qty;
                sell_order.Quantity -= fulfill_qty;

                if (OrderList[bid_id].Quantity == 0)
                    bid_queue.pop_front();
            }

            if (bid_queue.empty())
                Bids.erase(best_bid_price);
        }
    }

    void add_order(Order &order)
    {
        if (order.Ordertype == OrderType::Buy)
            match_buy(order);
        else
            match_sell(order);

        if (order.Quantity > 0){
            OrderList.insert({order.ID, order});

            if (order.Ordertype == OrderType::Buy)
                Bids[order.Price].push_back(order.ID);
            else
                Asks[order.Price].push_back(order.ID);
        }
    }
};

int main()
{
    return 0;
}