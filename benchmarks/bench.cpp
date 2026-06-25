#include <benchmark/benchmark.h>
#include "../include/OrderBook.h"

static void BM_MatchingEngine(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        
        OrderBook book;
        for (uint64_t i = 1; i <= 10000; ++i) {
            book.add_order({i, static_cast<int64_t>(100 + (i % 50)), 10, OrderType::Sell});
        }
        
        Order aggressive_buy = {99999, 150, 500, OrderType::Buy};
        
        state.ResumeTiming();
        
        book.add_order(aggressive_buy);
        
        benchmark::DoNotOptimize(book);
    }
}

BENCHMARK(BM_MatchingEngine);
BENCHMARK_MAIN();