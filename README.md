# Limit Order Book Engine

An order book written in C++17. Supports Market and Limit orders across various time-in-force policies, with a matching engine that sweeps the opposing book on aggressive incoming orders. Built to explore market microstructure and the foundational data structures powering financial exchanges.

## Features

* **Order Sides:** The engine supports both `Buy` and `Sell` operations.
* **Order Categories:** Users can submit `Limit` and `Market` orders. Market orders are clamped to extreme price boundaries to ensure immediate aggressive execution.
* **Time in Force (TIF) Instructions:**
  * **GTC (Good 'Til Canceled):** The order rests in the book until it is fully executed or explicitly canceled.
  * **IOC (Immediate Or Cancel):** The engine executes as much of the order quantity as possible immediately. Any remaining unfulfilled quantity is instantly canceled.
  * **FOK (Fill Or Kill):** The order requires the engine to execute the entire quantity immediately. If full execution is not possible, the order is completely canceled without partial fills.
* **Targeted Optimizations:**
  * **Pre-Allocated Memory Pool:** A custom pre-allocated memory pool manages order states, completely bypassing dynamic heap allocations during order ingestion to prevent latency spikes.
  * **O(1) Cancellations:** Integrates `robin_hood::unordered_flat_map` for instant order retrieval, combined with immediate bidirectional unlinking from the price-level queue.

## Architecture

The matching engine maintains state using the following containers:
* **Bids & Asks:** Price levels are stored in `absl::btree_map` instead of `std::map`. The B-Tree structure packs multiple keys per node, drastically reducing cache misses and improving sequential traversal speed during deep-book sweeps.
* **Price Level Queues:** Each unique price level maintains a doubly-linked list. This guarantees strict Time priority (FIFO) execution while avoiding the $\mathcal{O}(N)$ reallocation and pointer-chasing overhead of `std::deque`.
* **Order Tracking:** A central `robin_hood::unordered_flat_map` tracks all resting orders by mapping their `uint64_t` IDs directly to their index within the pre-allocated memory pool.

## Prerequisites

* **Compiler:** `g++` compiler with C++17 support.
* **Libraries:** Google Benchmark (`-lbenchmark`) and `pthread` are required for compiling the benchmarking executable. The Abseil library is required for `absl::btree_map`.

## Structure

    include/
      Types.h        -- Order struct, OrderType, TimeInForce, OrderCategory
      OrderBook.h    -- OrderBook class and core matching logic
      MemoryPool.h   -- Custom contiguous memory allocator for orders
      robin_hood.h   -- Fast hash map implementation
    src/
      main.cpp       -- Core logic assertions and test suite
    benchmarks/
      bench.cpp      -- Google Benchmark scenarios
    Makefile         -- Build configuration and targets

## Usage

    #include "include/OrderBook.h"
    #include "include/Types.h"

    int main() {
        // Pre-allocate the memory pool for up to 10,000 orders
        OrderBook book(10000);

        // Add resting sell orders
        book.add_order({1, 101, 50, OrderType::Sell, TimeInForce::GTC, OrderCategory::Limit});
        book.add_order({2, 102, 50, OrderType::Sell, TimeInForce::GTC, OrderCategory::Limit});

        book.add_order({3, 101, 30, OrderType::Buy, TimeInForce::GTC, OrderCategory::Limit});

        // O(1) cancellation unlinks the node immediately
        book.cancel_order(2);
        
        return 0;
    }

## Building the Project

To build both the testing and benchmarking executables, run:

    make all

This will create the `build/` directory and generate two binaries:
1. `build/OrderBookEngine` (Main testing binary)
2. `build/BenchEngine` (Benchmarking binary)

To clean up all compiled artifacts, run:

    make clean

## Testing & Benchmarks

The core engine logic is validated using built-in assertions located in `src/main.cpp`. The test suite verifies the correct behavior of the matching engine under different scenarios (GTC partial fills, IOC partial execution, FOK execution/rejection, and Market orders).

Execute the test suite:

    ./build/OrderBookEngine

Execute the benchmark suite:

    ./build/BenchEngine