#include <cstdint>
#include <string>

struct HandshakePacket {
  uint16_t protocolVersion;
  uint16_t pluginVersion;

  std::string serverName;
  std::string apiKey;
};
