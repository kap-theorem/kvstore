#include "kvstore.h"
#include <iostream>
#include <fstream>
#include "utils/string_utils.h"
#include "utils/stream_utils.h"
#include <chrono>

using namespace std;

KVStore::KVStore(const string& filename) {
    cout << "Memory Initialised\n";
    this->filename_ = filename;

    this->filename_ = filename;
    
    // Open the file for reading and writing in binary mode
    file_stream_.open(filename_, ios::in | ios::out | ios::binary);
    
    // If file doesn't exist, create it
    if (!file_stream_.is_open()) {
        file_stream_.clear();
        file_stream_.open(filename_, ios::out | ios::binary);
        if (!file_stream_.is_open()) {
            throw runtime_error("Failed to create file: " + filename_);
        }

        file_stream_.close();
        file_stream_.open(filename_, ios::in | ios::out | ios::binary);

        if (!file_stream_.is_open()) {
            throw runtime_error("Failed to open newly created file: " + filename_);
        }
    }
    
    LoadFromLog();

    background_writer_ = std::thread(&KVStore::BackgroundWriterLoop, this);
}

KVStore::~KVStore() {
    {
        std::lock_guard<std::mutex> lock(mtx_);
        stop_thread_ = true;
    }

    cv_.notify_one();

    if (background_writer_.joinable()) {
        background_writer_.join();
    }

    cout << "Memory Destroyed\n";
    if (file_stream_.is_open()) {
        file_stream_.flush();
        file_stream_.close();
    }
}

void KVStore::Put(const string& key, const string& value) {
    std::lock_guard<std::mutex> lock(mtx_);

    store[key] = value;

    pending_writes_.push_back(WriteOp{key, value});

    cv_.notify_one();
}

optional<string> KVStore::Get(const string& key) {
    std::lock_guard<std::mutex> lock(mtx_);

    if (store.find(key) == store.end()) {
        std::cout << key << " is no† present :(\n";
        return std::nullopt;
    }

    return store[key];
}

uint32_t KVStore::CalculateCheckSum(const string& key, const string& value) {
    uint32_t hash = 5381;

    for (char c: key) {
        hash = ((hash << 5) + hash) + c;
    }
    for (char c: value) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

void KVStore::BackgroundWriterLoop() {
    std::vector<WriteOp> batch;

    while (true) {
        {
            std::unique_lock<std::mutex> lock(mtx_);

            cv_.wait(lock, [this]() {
                return !pending_writes_.empty() || stop_thread_;
            });

            if (stop_thread_ && pending_writes_.empty()) {
                break;
            }

            batch.swap(pending_writes_);
        }

        for (const auto& op : batch) {
            LogHeader header;
            header.value_size = op.value.size();
            header.key_size = op.key.size();

            auto now = std::chrono::system_clock::now();
            header.timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
            header.checksum = CalculateCheckSum(op.key, op.value);

            file_stream_.write(reinterpret_cast<const char*>(&header), sizeof(LogHeader));
            file_stream_.write(op.key.c_str(), header.key_size);
            file_stream_.write(op.value.c_str(), header.value_size);
        }

        if (!batch.empty()) {
            file_stream_.flush();
            batch.clear();
        }
    }
}

void KVStore::LoadFromLog() {
    file_stream_.seekg(0, ios::beg);
    { StreamGaurd(file_stream_, "seek to beginning"); }

    while (true) {
        LogHeader header;

        // Read the header
        file_stream_.read(reinterpret_cast<char*>(&header), sizeof(LogHeader));
        { StreamGaurd(file_stream_, "read header"); }

        // If we hit the end of file, stop reading
        if (file_stream_.eof()) break;
        
        // If partial header is read, data is truncated or corrupted at the end
        if (file_stream_.gcount() != sizeof(LogHeader)) {
            cout << "Warning, Partial header read, log may be truncated" << endl;
            break;
        }

        // Reading key and value from the header
        string key(header.key_size, '\0');
        file_stream_.read(&key[0], header.key_size);
        { StreamGaurd(file_stream_, "read key"); }

        string value(header.value_size, '\0');
        file_stream_.read(&value[0], header.value_size);
        { StreamGaurd(file_stream_, "read value"); }

        // Verify checksum
        uint32_t calculated_checksum = CalculateCheckSum(key, value);
        if (calculated_checksum != header.checksum) {
            cout << "CRITICAL ERROR: Checksum not matching" << endl;
            cout << "Stopping ;pg replay at safe point" << endl;
            break;
        }

        store[key] = value;
    }

    file_stream_.clear();
    file_stream_.seekp(0, ios::end);
    { StreamGaurd(file_stream_, "Seek to end for appending"); }
    cout << "Successfully loaded and verified " << store.size() << " records." << endl;
}

void KVStore::Compact() {
    cout << "[Compaction] Starting background log rewrite...\n";

    string temp_filename = filename_ + ".tmp";

    ofstream tmp_file(temp_filename, ios::out | ios::binary);
    if (!tmp_file.is_open()) {
        cout << "[Compaction Error] Could not open temp file!\n";
        return;
    }

    for (const auto& [key, value]: store) {
        LogHeader header;
        header.key_size = key.size();
        header.value_size = value.size();

        auto now = chrono::system_clock::now();
        header.timestamp = chrono::duration_cast<chrono::seconds>(now.time_since_epoch()).count();
        header.checksum = CalculateCheckSum(key, value);

        tmp_file.write(reinterpret_cast<const char*>(&header), sizeof(LogHeader));
        { StreamGaurd(tmp_file, "Compaction: failed to write header"); }

        tmp_file.write(key.c_str(), header.key_size);
        { StreamGaurd(tmp_file, "Compaction: failed to write key"); }

        tmp_file.write(value.c_str(), header.value_size);
        { StreamGaurd(tmp_file, "Compaction: failed to write value"); }
    }

    tmp_file.flush();
    { StreamGaurd(tmp_file, "Compaction: failed to flush"); }
    tmp_file.close();
    
    if (file_stream_.is_open()) {
        file_stream_.close();
    }

    filesystem::rename(temp_filename, filename_);
    file_stream_.open(filename_, ios::in | ios::out | ios::app | ios::binary);

    cout << "[COMPACTION] Finished. Log size optimized.\n";
}