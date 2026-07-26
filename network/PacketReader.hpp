#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <vector>
class PacketReader {
private:
  size_t offset = 0;
  std::vector<char> buffer;

public:
  PacketReader(std::vector<char> buf) : buffer(buf) {}

  std::optional<uint32_t> readUInt32() {
    if (!(offset + sizeof(uint32_t) <= buffer.size())) {
      return std::nullopt;
    }

    uint32_t value;
    memcpy(&value, buffer.data() + offset, sizeof(uint32_t));
    offset += sizeof(uint32_t);
    return value;
  }

  std::optional<uint16_t> readUInt16() {
    if (!(offset + sizeof(uint16_t) <= buffer.size())) {
      return std::nullopt;
    }

    uint16_t value;
    memcpy(&value, buffer.data() + offset, sizeof(uint16_t));
    offset += sizeof(uint16_t);
    return value;
  }
};
