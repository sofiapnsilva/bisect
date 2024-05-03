#pragma once

#include "bisect/reactor/handler.h"
#include <memory>
#include <vector>
#include <thread>

namespace bisect::reactor
{
    /// Runs asynchronously and notifies registered handlers when their
    /// file descriptor is ready for reading.
    class reactor_t
    {
      public:
        /// Launches a thread that monitors the file descriptors of all currently registered handlers.
        /// That thread should call `handle_read` on all the handlers whose fd are ready for read.
        reactor_t();
        ~reactor_t();

        /// @brief Registers a new handler. New handlers may be registered at any time, concurrently,
        /// and should be monitored immediately.
        /// @param handler the abstract owner of the file descriptor.
        void register_handler(std::shared_ptr<handler_t> handler);

        /// @brief Unegisters an handler. This may be called at any time, concurrently,
        /// and the file descriptor should be removed from the monitored set before the function returns.
        void unregister_handler(int fd);

      private:
        std::vector<std::shared_ptr<handler_t>> handlers_;
        std::thread reactor_thread_;
        std::atomic<bool> running_;

        /// @brief Executes the main event loop of the reactor, continuously 
        /// monitoring registered handlers for read events.
        ///
        /// This function implements the core event-driven behavior of the reactor.
        /// It uses the `select()` system call to monitor
        /// a set of file descriptors associated with registered handlers for read readiness.
        /// When a file descriptor becomes ready
        /// for reading within the specified timeout period, the corresponding handler's 
        /// handle_read() method is invoked.
        ///
        /// The loop runs indefinitely while the running_ flag is set to true. 
        /// Each iteration of the loop involves:
        /// - Building a set of file descriptors (`read_fds`) based on registered handlers.
        /// - Using `select()` to wait for read events on these file descriptors with a 
        /// timeout of 0.5 seconds.
        /// - Processing any ready file descriptors by invoking their respective `handle_read()` methods.
        void reactor_loop();
    };
} // namespace bisect::reactor
