# Limit Order Book Engine

A Limit Order Book (LOB) engine implemented in C++17, designed for quantitative research and order execution workflows. This engine utilizes standard as well as specialized data structures to maintain price-time priority matching algorithm while supporting various order types and time-in-force instructions.

## Features

* **Order Sides:** The engine supports both `Buy` and `Sell` operations.
* **Order Categories:** Users can submit `Limit` and `Market` orders. Market orders are handled by assigning them an extreme price boundary. 
* **Time in Force (TIF) Instructions:**
  * **GTC (Good 'Til Canceled):** The order rests in the book until it is fully executed or explicitly canceled by the user.
  * **IOC (Immediate Or Cancel):** The engine executes as much of the order quantity as possible immediately. Any remaining unfulfilled quantity is instantly canceled.
  * **FOK (Fill Or Kill):** The order requires the engine to execute the entire quantity immediately. If full execution is not possible, the order is completely canceled without partial fills.
* **Targeted Optimizations:**
  * **Fast Lookups:** Integrates `robin_hood::unordered_flat_map` for O(1) order retrieval and state management.
  * **Lazy Deletion:** Canceling an order removes it from the hash map immediately while leaving the ID in the price-level queue. The matching loop skips these IDs, avoiding an O(n) queue scan on every cancellation.

## Architecture

The matching engine maintains state using the following containers:
* **Bids (Buy Orders):** Stored in a `std::map` and sorted in descending order using `std::greater<int64_t>` so the highest bid is matched first.
* **Asks (Sell Orders):** Stored in a standard `std::map`, naturally sorting in ascending order so the lowest ask is matched first.
* **Price Level Queues:** Each unique price level in the maps corresponds to a `std::deque<uint64_t>` holding order IDs. This ensures Time priority (FIFO) execution for orders at the same price.
* **Order Tracking:** A central `robin_hood::unordered_flat_map` tracks all resting orders by mapping their `uint64_t` IDs to their full `Order` structs.

## Prerequisites

* **Compiler:** `g++` compiler with C++17 support.
* **Libraries:** Google Benchmark (`-lbenchmark`) and `pthread` are required for linking and compiling the benchmarking executable.

## Structure

    include/
      Types.h        -- Order struct, OrderType, TimeInForce, OrderCategory
      OrderBook.h    -- OrderBook class and matching logic
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
        OrderBook book;

        // Add resting sell orders
        book.add_order({1, 101, 50, OrderType::Sell, TimeInForce::GTC, OrderCategory::Limit});
        book.add_order({2, 102, 50, OrderType::Sell, TimeInForce::GTC, OrderCategory::Limit});

        book.add_order({3, 101, 30, OrderType::Buy, TimeInForce::GTC, OrderCategory::Limit});

        // O(1) lazy cancellation of the remaining order
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
