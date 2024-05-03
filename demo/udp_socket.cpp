// udp_socket.cpp
#include "udp_socket.h"
#include <cstdio>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
using namespace bisect::demo;
using namespace bisect::reactor;

namespace bisect::demo
{
    std::shared_ptr<udp_socket_t> udp_socket_t::create_udp_socket(uint16_t port)
    {
        // Open the UDP socket and bind to the specified port
        int fd = open_socket(port);
        if (fd == -1)
        {
            std::fprintf(stderr, "Failed to create UDP socket\n");
            return nullptr; 
        }

        // Use std::make_shared with a custom deleter to create udp_socket_t instance
        return std::shared_ptr<udp_socket_t>(new udp_socket_t(fd),
                                              [](udp_socket_t* ptr) {
                                                  delete ptr; // Custom deleter to properly delete object
                                              });
    }

    udp_socket_t::udp_socket_t(int fd) : fd_(fd)
    {
    }

    udp_socket_t::~udp_socket_t()
    {
        // Close the socket when the object is destroyed
        if (fd_ != -1)
            close(fd_);
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


    int bind_to_port(int fd, uint16_t port)
    {
        struct sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(port);

        if (bind(fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) < 0)
        {
            // Binding failed
            perror("bind");
            std::fprintf(stderr, "Failed to bind UDP socket to port %d\n", port);
            close(fd);
            return -1; 
        }
        // Binding successful
        return 0; 
    }

    int open_socket(uint16_t port)
    {
        // Create a UDP socket
        int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

        if (fd == -1)
        {
            // Socket creation failed
            std::fprintf(stderr, "Failed to create UDP socket\n");
            return -1; 
        }

        // Bind the socket to the specified port
        if (bind_to_port(fd, port) < 0)
        {
            // Binding failed
            close(fd);
            return -1; 
        }

        // Set the socket to non-blocking mode
        if (fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK) == -1)
        {
            // Setting socket to non-blocking mode failed
            std::fprintf(stderr, "Failed to set socket to non-blocking mode\n");
            close(fd);
            return -1; 
        }

        // Return the socket file descriptor
        return fd; 
    }
}