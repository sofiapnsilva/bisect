#include "udp_socket.h"
#include <cstdio>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
using namespace bisect::demo;
using namespace bisect::reactor;

namespace
{
    int create_udp_socket()
    {
        return socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    }

    int bind_to_port(int fd, uint16_t port)
    {
        sockaddr_in addr{};
        addr.sin_family      = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port        = htons(port);

        if(bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
        {
            return -1;
        }

        return 0;
    }

    // Opens the socket and returns the fd.
    int open_socket(uint16_t port)
    {
        const auto fd = create_udp_socket();
        if(fd < 0) return fd;

        if(bind_to_port(fd, port) < 0) return -1;

        if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) == -1)
        {
            return -1;
        }

        return fd;
    }
} // namespace

udp_socket_t::udp_socket_t(uint16_t port) : fd_(open_socket(port))
{
}

bool udp_socket_t::is_valid() const noexcept
{
    return fd_ != -1;
}

int udp_socket_t::get_fd() noexcept
{
    return fd_;
}

void udp_socket_t::handle_read() noexcept
{
    printf("socket %d is ready for read\n", fd_);

    char buffer[1500];
    for(;;)
    {
        const auto r = read(fd_, buffer, sizeof(buffer));
        if(r < 0)
        {
            if(errno != EWOULDBLOCK)
            {
                printf("error reading from socket: %d\n", errno);
            }
            return;
        }

        printf("read %ld bytes from %d\n", r, fd_);
    }
}
