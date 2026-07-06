#include <benchmark/benchmark.h>
#include "../include/OrderBook.h"

static void BM_MatchingEngine(benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming();

        OrderBook book(10000);
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

static void BM_PriceLevelSweep(benchmark::State &state)
{
    for (auto _ : state)
    {
        state.PauseTiming();
        OrderBook book;

        for (uint64_t i = 1; i <= 5000; ++i)
        {
            book.add_order({i, static_cast<int64_t>(i), 1, OrderType::Sell, TimeInForce::GTC, OrderCategory::Limit});
        }

        Order sweep_buy = {99999, 5000, 1000, OrderType::Buy}; 
        state.ResumeTiming();

        book.add_order(sweep_buy);
        benchmark::DoNotOptimize(book);
    }
}


BENCHMARK(BM_MatchingEngine);
BENCHMARK(BM_PriceLevelSweep);
BENCHMARK_MAIN(); // always last