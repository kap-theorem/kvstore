#pragma once

#include <string>
#include <unordered_map>
#include <optional>

using namespace std;

class KVStore
{
public:
  KVStore(const string& filename);

  ~KVStore();

  void Put(const string& key, const string& value);

  optional<string> Get(const string& key);

private:
  unordered_map<string, string> store;
  string filename_;

  void LoadFromFile();
  void SaveToFile();
};