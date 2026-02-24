#pragma once

#include <string>
#include <unordered_map>
#include <optional>
#include <cstdint>
#include <fstream>
#include <condition_variable>
#include <thread>
#include <mutex> 

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
  void Compact();

private:
  unordered_map<string, string> store;
  string filename_;
  fstream file_stream_;

  struct WriteOp {
    std::string key;
    std::string value;
  };

  std::vector<WriteOp> pending_writes_;  // Queue for holding pending writes
  std::mutex mtx_;  // Used for protecting the map and queue
  std::condition_variable cv_;  // waking up the background thread
  std::thread background_writer_; // worker thread
  bool stop_thread_;  // flag to shutdown

  void LoadFromLog();

  void BackgroundWriterLoop();

  uint32_t CalculateCheckSum(const string& key, const string& value);
};