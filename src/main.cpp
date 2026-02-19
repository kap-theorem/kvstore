#include "kvstore.h"
#include <iostream>

using namespace std;

int main() {
    KVStore db("data/key_value.db");

    // db.Put("user1", "Pulasthya");
 
    db.Put("user1", "Pulasthya1");
    db.Put("user2", "Pulasthya2");
    db.Put("user3", "Pulasthya3");
    db.Put("user4", "Pulasthya4");
    db.Put("user5", "Pulasthya5");
    db.Put("user6", "Pulasthya6");

    auto result = db.Get("user1");

    if (result.has_value()) {
        cout << "Value: " << result.value() << "\n";
    }

    db.Compact();

    return 0;
}