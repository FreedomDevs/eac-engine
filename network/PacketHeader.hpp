#pragma once

#include <cstdint>

struct PacketHeader {
  uint32_t length;
  uint16_t type;
};
