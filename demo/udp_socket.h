#pragma once

#include "bisect/reactor/handler.h"
#include <cinttypes>
#include <memory>

namespace bisect::demo
{
    /// @brief Binds a socket file descriptor to a specified UDP port.
    /// @param fd The file descriptor of the socket to bind.
    /// @param port The UDP port number to bind the socket to.
    /// @return 0 on success, -1 on failure.
    int bind_to_port(int fd, uint16_t port);

    /// @brief Opens a UDP socket and binds it to the specified port.
    /// @param port The UDP port number to bind the socket to.
    /// @return The file descriptor (FD) of the opened and bound UDP socket, or -1 on failure.
    /// @note This function creates a UDP socket using the socket(AF_INET, SOCK_DGRAM, IPPROTO_IP) call,
    ///       then calls bind_to_port() to bind the socket to the specified UDP port.
    int open_socket(uint16_t port);

    class udp_socket_t : public reactor::handler_t
    {
      public:
        /// @brief Static factory function to create a UDP socket and bind it to a specified port.
        /// @param port The UDP port number to bind the socket to.
        /// @return A shared_ptr to the created udp_socket_t instance, or nullptr on failure.
        /// @note Enabling safe and managed ownership of udp_socket_t objects.
        static std::shared_ptr<udp_socket_t> create_udp_socket(uint16_t port);

        /// @brief Destructor for the udp_socket_t class.
        /// @details This destructor is responsible for cleaning up the udp_socket_t instance,
        ///          ensuring that the underlying socket file descriptor is properly closed.
        ~udp_socket_t();

        /// @brief Checks if the socket is valid.
        /// @return true if the socket is valid, false otherwise.
        bool is_valid() const noexcept;

        /// @brief Retrieves the file descriptor associated with the UDP socket.
        /// @return The file descriptor (`fd_`) associated with the UDP socket.
        int get_fd() noexcept override;

        /// @brief Handles read events on the UDP socket.
        /// @details This function is invoked when the UDP socket is ready for reading.
        ///          It attempts to read data from the socket into a buffer and processes
        ///          the received data. If no data is available or the read operation
        ///          encounters an error (excluding EWOULDBLOCK), an error message is
        ///          printed to the console. The function operates in a loop to handle
        ///          multiple read operations until an error occurs or no more data is
        ///          available.
        void handle_read() noexcept override;

      private:

        /// @brief Private constructor to enforce usage of create function.
        /// @param fd File descriptor
        explicit udp_socket_t(int fd);

        /// @brief the socket's file descriptor.
        const int fd_;

        // Deleted copy constructor and copy assignment operator
        // prevent accidental copying of udp_socket_t objects, 
        // enforcing the intended usage of std::shared_ptr for ownership.
        udp_socket_t(const udp_socket_t&) = delete;
        udp_socket_t& operator=(const udp_socket_t&) = delete;
    };
} // namespace bisect::demo
