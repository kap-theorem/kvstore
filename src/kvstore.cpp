#include "kvstore.h"
#include <iostream>

using namespace std;

KVStore::KVStore() {
    cout << "Memory Initialised\n";
}

KVStore::~KVStore() {
    cout << "Memory Destroyed\n";
}

void KVStore::Put(const string& key, const string& value) {
    store.insert({key, value});
}

optional<string> KVStore::Get(const string& key) {
    if (store.find(key) == store.end()) {
        std::cout << key << " is no† present :(\n";
        return std::nullopt;
    }

    return store[key];
}