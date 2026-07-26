#include <cstdint>

enum class PacketType : uint16_t {
  Handshake = 1,
  Ping = 2,
  Pong = 3,

  PlayerMove = 100,
  PlayerChat = 101,
};
