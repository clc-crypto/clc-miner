#include "uint256.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>

uint256::uint256() {
    data.fill(0);
}

uint256::uint256(const std::string& hexStr) {
    if (hexStr.length() != 64) throw std::invalid_argument("Hex string must be 64 characters.");
    for (size_t i = 0; i < 32; i++) {
        data[i] = std::stoi(hexStr.substr(i * 2, 2), nullptr, 16);
    }
}

bool uint256::operator<=(const uint256& other) const {
    return data <= other.data;
}

std::string uint256::toHex() const {
    std::ostringstream oss;
    for (uint8_t byte : data) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)byte;
    }
    return oss.str();
}
