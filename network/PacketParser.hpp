#include "PacketHeader.hpp"
#include "PacketReader.hpp"
#include <cstdint>
#include <cstring>
#include <optional>
#include <vector>
class PacketParser {
private:
  int offset;

public:
  struct ParsedPacket {
    std::uint16_t type;
    std::vector<char> payload;
  };

  std::optional<ParsedPacket> parsе(std::vector<char> &packet) {
    if (sizeof(PacketHeader) > packet.size()) {
      return std::nullopt;
    };
    PacketReader reader(packet);

    auto length = reader.readUInt32();
    if (!length) {
      return std::nullopt;
    }

    auto type = reader.readUInt16();
    if (!type) {
      return std::nullopt;
    }

    if (*length > packet.size() - sizeof(PacketHeader)) {
      return std::nullopt;
    }

    ParsedPacket pp;
    pp.type = *type;
    pp.payload =
        std::vector<char>(packet.begin() + sizeof(PacketHeader),
                          packet.begin() + sizeof(PacketHeader) + *length);

    return pp;
  };
};
