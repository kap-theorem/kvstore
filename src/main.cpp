#include "kvstore.h"
#include <iostream>

using namespace std;

int main() {
    KVStore db("data/key_value.txt");

    // db.Put("user1", "Pulasthya");

    auto result = db.Get("user1");

    if (result.has_value()) {
        cout << "Value: " << result.value() << "\n";
    }

    return 0;
}