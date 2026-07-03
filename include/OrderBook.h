#pragma once
#include <cstdint>
#include "robin_hood.h"
#include <map>
#include <algorithm>
#include "Types.h"
#include "MemoryPool.h"

class OrderBook
{
private:
    OrderMemoryPool memory_pool;
    robin_hood::unordered_flat_map<uint64_t, uint32_t> OrderList;
    std::map<int64_t, PriceLevel, std::greater<int64_t>> Bids;
    std::map<int64_t, PriceLevel> Asks;
    uint64_t internal_id = 1;

    void list_push_back(PriceLevel &level, uint32_t idx)
    {
        Node &n = memory_pool[idx];
        n.prev = level.tail;
        n.next = NULL_REF;

        if (level.tail != NULL_REF)
            memory_pool[level.tail].next = idx;
        else
            level.head = idx;

        level.tail = idx;
    }

    void list_remove(PriceLevel &level, uint32_t idx)
    {
        Node &n = memory_pool[idx];
        if (n.prev != NULL_REF)
            memory_pool[n.prev].next = n.next;
        else
            level.head = n.next;

        if (n.next != NULL_REF)
            memory_pool[n.next].prev = n.prev;
        else
            level.tail = n.prev;
    }

public:
    // Helpers for testing
    bool has_order(uint64_t id) const
    {
        return OrderList.find(id) != OrderList.end();
    }

    Order get_order(uint64_t id) const
    {
        return memory_pool[OrderList.at(id)].order;
    }

    // Main functions
    bool can_fill(Order &order)
    {
        uint64_t total_qty = 0;
        if (order.Ordertype == OrderType::Buy)
        {
            for (auto &[price, queue] : Asks)
            {
                if (price > order.Price)
                    break;

                for (uint32_t idx = queue.head; idx != NULL_REF; idx = memory_pool[idx].next)
                {
                    total_qty += memory_pool[idx].order.Quantity;
                    if (total_qty >= order.Quantity)
                        return true;
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

                for (uint32_t idx = queue.head; idx != NULL_REF; idx = memory_pool[idx].next)
                {
                    total_qty += memory_pool[idx].order.Quantity;
                    if (total_qty >= order.Quantity)
                        return true;
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

            while (ask_queue.head != NULL_REF && buy_order.Quantity > 0)
            {
                uint32_t idx = ask_queue.head;
                Node &n = memory_pool[idx];
                uint64_t fulfill_qty = std::min(n.order.Quantity, buy_order.Quantity);

                n.order.Quantity -= fulfill_qty;
                buy_order.Quantity -= fulfill_qty;

                if (n.order.Quantity == 0)
                {
                    uint64_t ask_id = n.order.ID;
                    list_remove(ask_queue, idx);
                    OrderList.erase(ask_id);
                    memory_pool.free_node(idx);
                }
            }

            if (ask_queue.head == NULL_REF)
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

            while (bid_queue.head != NULL_REF && sell_order.Quantity > 0)
            {
                uint32_t idx = bid_queue.head;
                Node &n = memory_pool[idx];
                uint64_t fulfill_qty = std::min(n.order.Quantity, sell_order.Quantity);

                n.order.Quantity -= fulfill_qty;
                sell_order.Quantity -= fulfill_qty;

                if (n.order.Quantity == 0)
                {
                    uint64_t ask_id = n.order.ID;
                    list_remove(bid_queue, idx);
                    OrderList.erase(ask_id);
                    memory_pool.free_node(idx);
                }
            }

            if (bid_queue.head == NULL_REF)
                Bids.erase(best_bid_price);
        }
    }

    uint64_t add_order(Order order)
    {
        order.ID = internal_id++;
        if (order.Category == OrderCategory::Market)
        {
            order.Price = (order.Ordertype == OrderType::Buy) ? 1000000000 : 0;
            if (order.TIF != TimeInForce::FOK)
            {
                order.TIF = TimeInForce::IOC;
            }
        }

        if (order.TIF == TimeInForce::FOK)
        {
            if (!can_fill(order))
            {
                return 0;
            }
        }

        if (order.Ordertype == OrderType::Buy)
            match_buy(order);
        else
            match_sell(order);

        if (order.Quantity > 0 && order.TIF == TimeInForce::GTC)
        {
            uint32_t idx = memory_pool.alloc_node(order);
            OrderList[order.ID] = idx;

            if (order.Ordertype == OrderType::Buy)
                list_push_back(Bids[order.Price], idx);
            else
                list_push_back(Asks[order.Price], idx);
        }

        return order.ID;
    }

    void cancel_order(uint64_t id)
    {
        auto it = OrderList.find(id);
        if (it == OrderList.end())
            return;

        uint32_t idx = it->second;
        Order &order = memory_pool[idx].order;

        if (order.Ordertype == OrderType::Buy)
        {
            auto lvl = Bids.find(order.Price);
            list_remove(lvl->second, idx);
            if (lvl->second.head == NULL_REF)
                Bids.erase(lvl);
        }
        else
        {
            auto lvl = Asks.find(order.Price);
            list_remove(lvl->second, idx);
            if (lvl->second.head == NULL_REF)
                Asks.erase(lvl);
        }

        OrderList.erase(it);
        memory_pool.free_node(idx);
    }
};