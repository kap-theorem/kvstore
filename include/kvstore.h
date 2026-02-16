#pragma once

#include <string>
#include <unordered_map>
#include <optional>

class KVStore
{
public:
  KVStore();

  ~KVStore();

  void Put(const std::string& key, const std::string& value);

  std::optional<std::string> Get(const std::string& key);

private:
  std::unordered_map<std::string, std::string> store;
};