#include "network/initSocket.h"
#include <iostream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>
#include <vector>

#define PORT 9090

struct Session {
  int fd;
  std::vector<uint8_t> buffer;
};

std::unordered_map<int, Session> sessions;

int main(void) {
  int tcpfd = initTcp(PORT);
  std::cout << "HelloWorld" << std::endl;

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
        }
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
        } else {
        }
      }
    }
  }
}
