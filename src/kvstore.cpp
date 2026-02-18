#include "kvstore.h"
#include <iostream>
#include <fstream>
#include "utils/string_utils.h"

using namespace std;

KVStore::KVStore(const string& filename) {
    cout << "Memory Initialised\n";
    this->filename_ = filename;
    LoadFromFile();
}

KVStore::~KVStore() {
    cout << "Memory Destroyed\n";
    SaveToFile();
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

void KVStore::LoadFromFile() {
    ifstream file(filename_);

    if (!file.is_open()) {
        cout << "File not present\n";
    } else {
        string key, value;
        while (file >> key) {
            getline(file, value);

            // Cleaning the I\Ps
            key = kvstore::utils::Trim(key);
            value = kvstore::utils::Trim(value);
            store.insert({key, value});
        }
    }

    file.close();
}

void KVStore::SaveToFile() {
    ofstream file(filename_);

    if (file.is_open()) {
        for (const auto& pair: store) {
            file << pair.first << " " << pair.second << endl;
        }

        file.close();
        cout << "Data saved to " << filename_ << endl;
    } else {
        cout << "Error in opening file" << endl;
    }
}