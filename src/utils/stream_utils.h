#pragma once
#include <fstream>
#include <string>
#include <stdexcept>
#include <functional>

using namespace std;

class StreamGaurd {

private:
    fstream& stream_;
    string operation_;

public:
    StreamGaurd(fstream& stream, string op)
        : stream_(stream), operation_(move(op)) {}

    ~StreamGaurd() {
        if (!stream_) {
            throw runtime_error(operation_ + " failed");
        }
    }
};