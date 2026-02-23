#pragma once
#include <fstream>
#include <string>
#include <stdexcept>
#include <functional>

using namespace std;

template <typename StreamType>
class StreamGaurd {

private:
    StreamType& stream_;
    string operation_;

public:
    StreamGaurd(StreamType& stream, string op)
        : stream_(stream), operation_(move(op)) {}

    ~StreamGaurd() {
        if (!stream_) {
            throw runtime_error(operation_ + " failed");
        }
    }
};