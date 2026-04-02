/**
 * @file mom_ng_main.cpp
 * @brief Entry point for the modernized Torque-NG Resource Manager (Mom).
 * * This file initializes the Boost.Asio io_context, sets up a multi-threaded
 * execution environment, and launches the asynchronous coroutine listeners
 * for handling node and job updates.
 * * @copyright
 * Copyright (c) 2026 Kenneth Nielson. All rights reserved.
 * Distributed under the terms of the adaptive Open Source License
 * specifically tailored for the Torque-NG project.
 */

#include <boost/asio.hpp>
#include <csignal>
#include <iostream>
#include <thread>
#include <vector>

// Placeholder for your Mmap and Manager headers
// #include "Libcommon/MmapBuffer.hpp"
// #include "Libcommon/NodeStatusManager.hpp"

namespace asio = boost::asio;

/**
 * @brief Global execution engine for the Torque-NG Mom.
 * * The io_context is the core event loop that manages coroutine suspension,
 * resumption, and all asynchronous I/O operations.
 */
asio::io_context io_ctx;

/**
 * @brief Handles OS signals for graceful shutdown.
 * * @param signals The signal set to monitor.
 */
asio::awaitable<void> watch_signals(asio::signal_set &signals) {
  co_await signals.async_wait(asio::use_awaitable);
  std::cout << "\n[Torque-NG] Shutdown signal received. Closing server..."
            << std::endl;
  io_ctx.stop();
}

/**
 * @brief Main execution entry point.
 * * Initializes the thread pool and starts the asynchronous engine.
 * * @return int Status code (0 for success, 1 for failure).
 */
int main() {
  try {
    // 1. Ensure the io_context has a work guard to prevent premature exit.
    auto work_guard = asio::make_work_guard(io_ctx);

    // 2. Set up signal handling (SIGINT for Ctrl+C, SIGTERM for system kill)
    asio::signal_set signals(io_ctx, SIGINT, SIGTERM);
    asio::co_spawn(io_ctx, watch_signals(signals), asio::detached);

    // 3. Initialize the Thread Pool
    // Each thread in the pool calls io_ctx.run() to process available
    // coroutines.
    unsigned int thread_count = std::thread::hardware_concurrency();
    if (thread_count == 0)
      thread_count = 2; // Fallback for single-core machines

    std::vector<std::thread> pool;
    std::cout << "--------------------------------------------------"
              << std::endl;
    std::cout << " Torque-NG Resource Manager (Mom) starting..." << std::endl;
    std::cout << " Execution Mode: C++20 Coroutines" << std::endl;
    std::cout << " Thread Pool Size: " << thread_count << std::endl;
    std::cout << "--------------------------------------------------"
              << std::endl;

    for (unsigned int i = 0; i < thread_count; ++i) {
      pool.emplace_back([]() { io_ctx.run(); });
    }

    // 4. Block the main thread until the io_context is stopped (via signals or
    // error)
    for (auto &t : pool) {
      if (t.joinable()) {
        t.join();
      }
    }

    std::cout << "[Torque-NG] Clean shutdown complete." << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "[Torque-NG] Fatal Initialization Error: " << e.what()
              << std::endl;
    return 1;
  }

  return 0;
}