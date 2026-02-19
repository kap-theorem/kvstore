#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <cstdint>
#include <fstream>

using namespace std;

#pragma pack(push, 1)
struct LogHeader
{
  uint32_t checksum;
  uint64_t timestamp;
  uint32_t key_size;
  uint32_t value_size;
};
#pragma pack(pop)

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
  fstream file_stream_;

  void LoadFromLog();
  void AppendLog(const string& key, const string& value);

  uint32_t CalculateCheckSum(const string& key, const string& value);
};