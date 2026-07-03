#include <cstdint>
#include <vector>
#include "Types.h"

constexpr uint32_t NULL_REF = 0xFFFFFFFFu;

struct Node{
    Order order;
    uint32_t prev = NULL_REF;
    uint32_t next = NULL_REF;
};

struct PriceLevel{
    uint32_t head = NULL_REF;
    uint32_t tail = NULL_REF;
};

class OrderMemoryPool{
private:
    std::vector<Node> pool;
    uint32_t free_head = NULL_REF;

public:
    Node &operator[](uint32_t idx){
        return pool[idx];
    }
    const Node &operator[](uint32_t idx) const{
        return pool[idx];
    }

    uint32_t alloc_node(const Order &order){
        uint32_t idx;

        if (free_head != NULL_REF){
            idx = free_head;
            free_head = pool[idx].next;
        }
        else{
            idx = static_cast<uint32_t>(pool.size());
            pool.emplace_back();
        }
        pool[idx].order = order;
        pool[idx].prev = NULL_REF;
        pool[idx].next = NULL_REF;

        return idx;
    }

    void free_node(uint32_t idx){
        pool[idx].next = free_head;
        free_head = idx;
    }
};