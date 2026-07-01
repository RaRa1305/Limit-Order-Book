#include <benchmark/benchmark.h>
#include "../include/OrderBook.h"

static void BM_MatchingEngine(benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming();

        OrderBook book;
        for (uint64_t i = 1; i <= 10000; ++i)
        {
            book.add_order({i, static_cast<int64_t>(100 + (i % 50)), 10, OrderType::Sell, TimeInForce::GTC, OrderCategory::Limit});
        }

        Order sweep_buy = {99999, 150, 500, OrderType::Buy};

        state.ResumeTiming();

        book.add_order(sweep_buy);
        benchmark::DoNotOptimize(book);
    }
}

static void BM_OrderCancel(benchmark::State& state) {
    uint64_t id = 1;
    
    for (auto _ : state) {
        state.PauseTiming();
        OrderBook book;
        // fresh book every iteration with one order to cancel
        book.add_order({id, 100, 10, OrderType::Sell, TimeInForce::GTC, OrderCategory::Limit});
        state.ResumeTiming();

        book.cancel_order(id);
        benchmark::DoNotOptimize(book);
        id++;
    }
}

BENCHMARK(BM_MatchingEngine);
BENCHMARK(BM_OrderCancel);
BENCHMARK_MAIN(); // always last