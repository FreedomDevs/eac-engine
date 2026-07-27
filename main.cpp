#include "network/PacketParser.hpp"
#include "network/PacketType.hpp"
#include "network/initSocket.h"
#include <fcntl.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#define PORT 3267
#define CONFIG_FILE "config.json"

struct Session {
  int fd;
  std::vector<char> buffer;

  bool handshaked = false;
  std::string serverName;
  uint16_t protocolVersion;
};

std::unordered_map<int, Session> sessions;

void createConfig() {
  int filefd = open(CONFIG_FILE, O_RDONLY);
  if (filefd >= 0) {
    close(filefd);
    return;
  }

  int newFilefd = open(CONFIG_FILE, O_CREAT | O_WRONLY, 0644);
  if (newFilefd < 0) {
    return;
  }

  nlohmann::json j;
  j["port"] = 3267;
  j["servers"] = nlohmann::json::array();

  j["servers"].push_back({{"name", "Survival"}, {"apiKey", "secret-key"}});

  std::string data = j.dump(2);
  write(newFilefd, data.c_str(), data.size());
  return;
}

int getPort() {
  int filefd = open(CONFIG_FILE, O_RDONLY);
  if (filefd < 0) {
    return -1;
  }

  struct stat st;
  if (fstat(filefd, &st) < 0) {
    close(filefd);
    return -1;
  }

  ssize_t size = st.st_size;

  std::vector<char> buf(size);
  ssize_t n = read(filefd, buf.data(), buf.size());

  if (n < 0) {
    return -1;
  }
  close(filefd);

  std::string jsonText(buf.begin(), buf.end());
  nlohmann::json j = nlohmann::json::parse(jsonText);

  int port = j["port"];
  return port;
}

int main(void) {
  createConfig();

  int port = getPort();
  if (port < 0) {
    std::cout << "Ошибка чтение порта" << std::endl;
  }

  int tcpfd = initTcp(port);
  std::cout << "EAC-Engine started: " << port << std::endl;

  int epfd = epoll_create(32);

  struct epoll_event ev;
  ev.events = EPOLLIN;
  ev.data.fd = tcpfd;
  epoll_ctl(epfd, EPOLL_CTL_ADD, tcpfd, &ev);

  struct epoll_event events[32];
  while (1) {
    int count = epoll_wait(epfd, events, 32, -1);

    for (int i = 0; i < count; i++) {
      int fd = events[i].data.fd;

      if (fd == tcpfd) {
        int clientfd = accept4(fd, NULL, NULL, SOCK_NONBLOCK | SOCK_CLOEXEC);
        if (clientfd < 0) {
          perror("client error");
          continue;
        }

        sessions.emplace(clientfd, Session{.fd = clientfd});

        epoll_event ev{};
        ev.events = EPOLLIN;
        ev.data.fd = clientfd;

        epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &ev);
      } else {
        Session &session = sessions[fd];
        char buffer[4096];

        int n = recv(fd, buffer, sizeof(buffer), 0);

        if (n == 0) {
          epoll_ctl(epfd, EPOLL_CTL_DEL, fd, nullptr);
          close(fd);
          sessions.erase(fd);
        } else if (n > 0) {
          session.buffer.insert(session.buffer.end(), buffer, buffer + n);
          PacketParser pars;

          while (1) {
            auto packet = pars.parsе(session.buffer);

            if (!packet) {
              break;
            }

            PacketType type = static_cast<PacketType>(packet->type);

            if (!session.handshaked && type != PacketType::Handshake) {
              close(fd);
              sessions.erase(fd);
              break;
            }

            if (type == PacketType::Handshake) {
            }
          }
        } else {
        }
      }
    }
  }
}
