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
    HashNode* new_node = new HashNode(v);
    new_node->next = this;
    return new_node;
  }

};

template <class K, class V>
struct HashMap {
  std::unordered_map<K, HashNode<V>* > hash_map;

  int size() {
    return hash_map.size();
  }

  void insert(K key, V value) {
    auto got = hash_map.find(key);
    if (got != hash_map.end()) {
      hash_map[key] = hash_map[key]->list_add(value);
    } else {
      hash_map[key] = new HashNode<V>(value);
    }
    return;
  }

  HashNode<V>* find(K key) {
    auto got = hash_map.find(key);
    if (got != hash_map.end()) {
      return got->second;
    }
    return NULL;
  }

  void reset() {
    for (auto item : hash_map) {
      HashNode<V>* node = item.second;
      while (node != NULL) {
	HashNode<V>* next_node = node->next;
	delete node;
	node = next_node;
      }
    }
    hash_map.clear();
  }
};

uint64_t hash_3int16(uint16_t x, uint16_t y, uint16_t z) {
  return ((uint64_t)x << 32) + ((uint64_t)y << 16) + (uint64_t)z;
}

uint64_t hash_3int16_2(uint16_t x, uint16_t y, uint16_t z) {
  return (((uint64_t)x*73856093)^((uint64_t)y*19349663)^((uint64_t)z*83492791))%137216;
}

uint64_t hash_3int16_3(uint16_t x, uint16_t y, uint16_t z) {
  return ((uint64_t)x * 1000000007 + y) * 1000000007 + z;
}
