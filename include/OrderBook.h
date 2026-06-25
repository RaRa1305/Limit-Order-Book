#pragma once
#include <cstdint>
#include <unordered_map>
#include <map>
#include <deque>
#include <vector>
#include <algorithm>
#include "Types.h"

class OrderBook
{
private:
    std::unordered_map<uint64_t, Order> OrderList;
    std::map<int64_t, std::deque<uint64_t>, std::greater<int64_t>> Bids;
    std::map<int64_t, std::deque<uint64_t>> Asks;

public:
    // Helpers for testing
    bool has_order(uint64_t id) const {
        return OrderList.find(id) != OrderList.end();
    }

    Order get_order(uint64_t id) const {
        return OrderList.at(id);
    }

    //Main functions 
    bool can_fill(Order &order)
    {
        uint64_t total_qty = 0;
        if (order.Ordertype == OrderType::Buy)
        {
            for (auto &[price, queue] : Asks)
            {
                if (price > order.Price)
                    break;

                for (uint64_t id : queue)
                {
                    auto it = OrderList.find(id);
                    if (it != OrderList.end())
                    {
                        total_qty += it->second.Quantity;
                        if (total_qty >= order.Quantity)
                            return true;
                    }
                }
            }
            return false;
        }
        else
        {
            for (auto &[price, queue] : Bids)
            {
                if (price < order.Price)
                    break;

                for (uint64_t id : queue)
                {
                    auto it = OrderList.find(id);
                    if (it != OrderList.end())
                    {
                        total_qty += it->second.Quantity;
                        if (total_qty >= order.Quantity)
                            return true;
                    }
                }
            }
            return false;
        }
    }

    void match_buy(Order &buy_order)
    {
        while (!Asks.empty() && buy_order.Quantity > 0)
        {
            auto best_ask = Asks.begin();
            int64_t best_ask_price = best_ask->first;
            auto &ask_queue = best_ask->second;

            if (buy_order.Price < best_ask_price)
                break;

            while (!ask_queue.empty() && buy_order.Quantity > 0)
            {
                uint64_t ask_id = ask_queue.front();
                auto it = OrderList.find(ask_id);

                if (it != OrderList.end())
                {
                    Order &resting_ask = it->second;
                    uint64_t fulfill_qty = std::min(resting_ask.Quantity, buy_order.Quantity);

                    resting_ask.Quantity -= fulfill_qty;
                    buy_order.Quantity -= fulfill_qty;

                    if (resting_ask.Quantity == 0)
                    {
                        ask_queue.pop_front();
                        OrderList.erase(ask_id);
                    }
                }
                else
                {
                    ask_queue.pop_front();
                }
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

            while (!bid_queue.empty() && sell_order.Quantity > 0)
            {
                uint64_t bid_id = bid_queue.front();
                auto it = OrderList.find(bid_id);

                if (it != OrderList.end())
                {
                    Order &resting_bid = it->second;
                    uint64_t fulfill_qty = std::min(resting_bid.Quantity, sell_order.Quantity);

                    resting_bid.Quantity -= fulfill_qty;
                    sell_order.Quantity -= fulfill_qty;

                    if (resting_bid.Quantity == 0)
                    {
                        bid_queue.pop_front();
                        OrderList.erase(bid_id);
                    }
                }
                else
                {
                    bid_queue.pop_front(); // Support for O(1) cancellation
                }
            }

            if (bid_queue.empty())
                Bids.erase(best_bid_price);
        }
    }

    void add_order(Order order)
    {
        if (order.Category == OrderCategory::Market)
        {
            order.Price = (order.Ordertype == OrderType::Buy) ? 1000000000 : 0;
            order.TIF = TimeInForce::IOC;
        }

        if (order.TIF == TimeInForce::FOK)
        {
            if (!can_fill(order))
            {
                return;
            }
        }

        if (order.Ordertype == OrderType::Buy)
            match_buy(order);
        else
            match_sell(order);

        if (order.Quantity > 0 && order.TIF == TimeInForce::GTC)
        {
            OrderList.insert({order.ID, order});

            if (order.Ordertype == OrderType::Buy)
                Bids[order.Price].push_back(order.ID);
            else
                Asks[order.Price].push_back(order.ID);
        }
    }
};