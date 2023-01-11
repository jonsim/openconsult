#ifndef OPENCONSULT_CONNECTION_H
#define OPENCONSULT_CONNECTION_H

#include <cstdint>
#include <vector>

namespace openconsult {

class Command {
public:
    virtual std::vector<uint8_t> commandSequence() = 0;
};

class Connection {
public:
    Connection();
    virtual ~Connection() = default;
};

}

#endif