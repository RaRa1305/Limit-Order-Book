CXX = g++
CXXFLAGS = -std=c++17 -O3 -march=native -Iinclude
LDFLAGS = -lbenchmark -lpthread

MAIN_OUT = build/OrderBookEngine
BENCH_OUT = build/BenchEngine

all: $(MAIN_OUT) $(BENCH_OUT)

$(MAIN_OUT): src/main.cpp include/OrderBook.h include/Types.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) src/main.cpp -o $(MAIN_OUT)
	@echo "Built $(MAIN_OUT)"

$(BENCH_OUT): benchmarks/bench.cpp include/OrderBook.h include/Types.h
	@mkdir -p build
	$(CXX) $(CXXFLAGS) benchmarks/bench.cpp -o $(BENCH_OUT) $(LDFLAGS)
	@echo "Built $(BENCH_OUT)"

clean:
	rm -rf build/*
	@echo "Cleaned build directory"