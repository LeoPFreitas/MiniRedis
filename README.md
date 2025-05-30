# MiniRedis – Lightweight In-Memory Key-Value Store in C++

## Project Overview

MiniRedis is a lightweight, Redis-inspired in-memory key-value store implemented in modern C++.
It supports basic commands (SET, GET, DEL), optional time-to-live (TTL) for keys, concurrent access, and a simple CLI
client.
The project is designed to simulate a real-world backend component with a focus on performance, memory management, and
software quality practices like testing, CI/CD, and documentation.

> **Project Details**
> - **Target duration:** 100 hours
> - **Tech stack:** Modern C++ (C++23), CMake, Google Test, clang-format, Doxygen
> - **Outcome:** A clean, tested, well-documented, and portable C++ application with CI/CD pipelines.

## Goals

### Primary Goals

- Learn and apply modern C++ syntax and idioms
- Build a performant and thread-safe in-memory key-value store
- Set up a professional development workflow (CI/CD, tests, docs)
- Practice writing maintainable and extensible C++ code

### Secondary Goals

- Understand and apply C++ memory and resource management (RAII, smart pointers)
- Get familiar with modern concurrency primitives in C++
- Work with third-party libraries and tools (Google Test, Doxygen, clang-tidy)

## Functional Requirements

### Basic Commands

- SET key value [ttl] – Store a key with optional TTL (in seconds)
- GET key – Retrieve the value for a key
- DEL key – Delete a key
- EXISTS key – Check if a key exists

### TTL (Optional per key)

- Keys with TTL auto-expire
- Background thread handles expiration

### Concurrency

- Concurrent reads and writes (thread-safe)
- Fine-grained locking or lock-free structures for scalability (e.g., std::shared_mutex)

### Command-Line Interface (CLI)

- Issue commands via a simple REPL client
- Local communication only (networking optional)

## Non-Functional Requirements

- Performance: Handle at least 10k ops/sec on a modern CPU
- Thread safety: Must be safe under concurrent access
- Portability: Linux/macOS (Windows optional)
- Testability: 80%+ unit test coverage
- Maintainability: Documented APIs and clean architecture

## Testing Strategy

- Unit tests for storage engine and TTL logic
- Integration tests for command sequences
- Concurrent test cases (race conditions, TTL accuracy)
- Use Google Test + CMake for test execution

## CI/CD Pipeline

- Trigger: On push and pull request
- Steps:
    - Build (with CMake)
    - Run unit tests
    - Run static analysis (clang-tidy, cppcheck)
    - Format check (clang-format)
    - Generate documentation (Doxygen)

## Benchmark Testing

- Ops/sec for GET, SET, and DEL
- Latency (avg, p95, p99)
- TTL expiration efficiency
- Concurrency performance (scaling with threads)

### Benchmark Reporting

You can optionally generate and track benchmark reports using:

- Markdown-based reports in the /docs/ folder
- Plotting tools like Python + matplotlib for historical performance
- Auto-run benchmarks in CI (GitHub Actions) with job output summary

### Performance Optimization Areas

As you benchmark and profile, focus on:

- Lock contention (mutex bottlenecks)
- Memory allocations (avoid frequent heap allocations)
- Cache locality (use std::unordered_map, align data)
- Smart usage of threads or thread pools