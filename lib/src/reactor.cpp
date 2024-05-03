#include "bisect/reactor/reactor.h"
using namespace bisect::reactor;

// Constructor initializes the reactor and starts its event 
// loop in a separate thread.
reactor_t::reactor_t() : running_(true)
{
    // Launch a new thread to run the reactor event loop.
    reactor_thread_ = std::thread([this] { reactor_loop(); });
}

// Destructor stops the reactor by setting the running flag to false 
// and joins the reactor thread.
reactor_t::~reactor_t()
{
    running_ = false;
    if (reactor_thread_.joinable())
        reactor_thread_.join();
}

// Registers a handler with the reactor to monitor its file descriptor for read events.
void reactor_t::register_handler(std::shared_ptr<handler_t> handler)
{
    handlers_.push_back(handler);
}

// Unregisters a handler associated with the specified file descriptor from the reactor.
void reactor_t::unregister_handler(int fd)
{
    handlers_.erase(
        std::remove_if(handlers_.begin(), handlers_.end(),
                        [fd](const auto& handler) { return handler->get_fd() == fd; }),
        handlers_.end());
}

// Main event loop of the reactor, which continuously monitors 
// registered handlers for read events.
void reactor_t::reactor_loop()
{
    fd_set read_fds;
    while (running_)
    {
        // Clear the set of file descriptors to monitor for read events.
        FD_ZERO(&read_fds);
        int max_fd = -1;

        // Add file descriptors of all registered handlers 
        // to the set and determine the maximum file descriptor.
        for (const auto& handler : handlers_)
        {
            int fd = handler->get_fd();
            FD_SET(fd, &read_fds);
            if (fd > max_fd)
                max_fd = fd;
        }

        // Set timeout for the select call to 0.5 seconds.
        struct timeval timeout = {0, 500000};

        // Wait for read events on the monitored file descriptors 
        // with the specified timeout.
        int ready = select(max_fd + 1, &read_fds, nullptr, nullptr, &timeout);

        // Check for errors in select call.
        if (ready < 0)
        {
            perror("Error in select call");
            continue;
        }

        // Check if select returned with a positive value (number of ready file descriptors).
        if (ready > 0)
        {
            // Iterate over all registered handlers to check which file 
            // descriptors are ready for reading.
            for (const auto& handler : handlers_)
            {
                int fd = handler->get_fd();
                // Check if the file descriptor is in the set of ready file descriptors.
                if (FD_ISSET(fd, &read_fds))
                {
                    // Invoke handle_read() on the handler associated 
                    // with the ready file descriptor.
                    handler->handle_read();
                }
            }
        }
    }
}

