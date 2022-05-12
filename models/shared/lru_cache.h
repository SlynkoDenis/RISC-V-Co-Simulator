#ifndef HW_CO_SIMULATION_LRU_CACHE_H
#define HW_CO_SIMULATION_LRU_CACHE_H

#include <list>
#include "macros.h"
#include <unordered_map>
#include <utility>
#include <vector>

namespace cache {
enum class LRUErrors {
    ERROR_NONE=0,
    NOT_FOUND,
};

template <typename K, typename V>
class LRUCache {
public:
    LRUCache(size_t max_sz): max_size_(max_sz) {}
    LRUCache(): max_size_(0) {}
    // TODO: implement COPY/MOVE semantics
    NO_COPY_SEMANTIC(LRUCache);
    NO_MOVE_SEMANTIC(LRUCache);
    virtual ~LRUCache() noexcept = default;

    virtual std::variant<LRUErrors, V> Get(K key) {
        auto iter = key_to_iters_.find(key);
        if (iter != key_to_iters_.end()) {
            values_.splice(values_.begin(), values_, iter->second);
            return {iter->second->second};
        }
        return {LRUErrors::NOT_FOUND};
    }
    virtual void Insert(K key, V value) {
        ASSERT(key_to_iters_.find(key) == key_to_iters_.end());
        if (values_.size() >= max_size_) {
            auto lru_key = values_.begin()->first;
            values_.pop_front();
            key_to_iters_.erase(lru_key);
        }
        values_.emplace_front(key, value);
        key_to_iters_.emplace(key, values_.begin());
    }

    void SetMaxSize(size_t max_sz) {
        max_size_ = max_sz;
    }

private:
    size_t max_size_;
    std::list<std::pair<K, V>> values_;
    std::unordered_map<K, typename std::list<std::pair<K, V>>::iterator> key_to_iters_;
};
}   // end namespace cache

#endif // HW_CO_SIMULATION_LRU_CACHE_H
