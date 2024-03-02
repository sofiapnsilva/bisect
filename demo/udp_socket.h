#pragma once

#include "bisect/reactor/handler.h"
#include <cinttypes>

namespace bisect::demo
{
    class udp_socket_t : public reactor::handler_t
    {
      public:
        /// @brief Creates a sockets that listens on all interfaces and binds to `port`.
        /// @param port the port to bind to.
        /// @note No error handling, so there is no feedback if construction fails.
        udp_socket_t(uint16_t port);

        /// NOTE:
        /// A FEW THINGS ARE MISSING HERE

        /// @brief Checks if the socket is valid.
        /// @return true if the socket is valid, false otherwise.
        bool is_valid() const noexcept;

        int get_fd() noexcept override;

        void handle_read() noexcept override;

      private:
        /// @brief the socket's file descriptor.
        const int fd_;
    };
} // namespace bisect::demo
