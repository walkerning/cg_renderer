#pragma once
#include <functional>
#include <unordered_map>

template <class V>
struct HashNode {
  size_t key;
  V value;
  HashNode* next;

  HashNode(V value_): value(value_), next(NULL) {}

  HashNode* list_add(V v) {
    next = new HashNode(v);
    return this;
  }

};

template <class K, class V>
struct HashMap {
  std::unordered_map<K, HashNode<V>* > hash_map;

  void insert(K key, V value) {
    auto got = hash_map.find(key);
    if (got != hash_map.end()) {
      hash_map[key] = hash_map[key]->list_add(value);
    }
    hash_map[key] = new HashNode<V>(value);
  }

  HashNode<V>* find(K key) {
    auto got = hash_map.find(key);
    if (got != hash_map.end()) {
      return got->second;
    }
    return NULL;
  }
};

uint64_t hash_3int16(uint16_t x, uint16_t y, uint16_t z) {
  return ((uint64_t)x << 32) + ((uint64_t)y << 16) + (uint64_t)z;
}
