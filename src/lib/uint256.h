#ifndef UINT256_H
#define UINT256_H

#include <array>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

class uint256 {
private:
    std::array<uint8_t, 32> data;

public:
    uint256();
    explicit uint256(const std::string& hexStr);
    bool operator<=(const uint256& other) const;
    std::string toHex() const;
};

#endif
