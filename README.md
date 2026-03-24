# Torque-NG (Next Generation)

[![License](https://img.shields.io/badge/License-TORQUE_v2.5-blue.svg)](LICENSE)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-orange.svg)](https://en.cppreference.com/w/cpp/17)
[![Platform](https://img.shields.io/badge/Platform-Linux-lightgrey.svg)](https://www.kernel.org/)
[![Build Status](https://img.shields.io/badge/Tests-6%2F6%20Passing-success.svg)](#-current-status--testing)

## ⚠️ Project Status: Active Development (Alpha)
**Torque-NG is in the "Grand Refactor" phase.** While the higher-level daemon logic is being integrated, the core protocol and utility libraries are now stable and verified. This project transitions the legacy C codebase into a type-safe, performant C++17 ecosystem.

---

## 📖 Project Overview & Vision

Torque-NG is an ambitious revival of the original TORQUE Resource Manager. TORQUE has historically been a cornerstone of HPC, originating from the original PBS resource manager developed by NASA, LLNL, and MRJ.

The project incorporates significant advancements in scalability and fault-tolerance contributed by organizations such as OSC, NCSA, TeraGrid, the U.S. Dept of Energy, USC, and many others. Our mission is to modernize this foundation for:
* **Modern Linux Kernels**: Native, first-class integration with Cgroup V2.
* **Type-Safe Communications**: Replacing legacy DIS protocols with Google Protocol Buffers.
* **Developer Productivity**: A clean, C++17 codebase that is easy to audit and maintain.

---

## 🌟 Key Modernizations

* **Protocol Buffers Integration**: Migrated communication logic to Protobuf for robust, language-agnostic serialization and structured message handling.
* **Scoped Type Safety**: Eliminated "primitive obsession" by replacing integer-based states with scoped Enums (`JobState`, `JobSubState`).
* **C++17 Architecture**: Replaced manual memory management with RAII, standard library containers, and modern thread-safe patterns.
* **Legacy Bridge**: Implemented a centralized `JobStateMapper` to handle the transition between legacy Torque string codes and modern internal types.
* **Linux Cgroup V2**: Native support for the unified control group hierarchy, providing superior resource isolation.
* **Topology Awareness**: Integrated with **hwloc 2.x** for intelligent scheduling across P/E-core and ARM architectures.

---

## 🧪 Current Status & Testing

The core protocol logic is now verified through a comprehensive GoogleTest suite.

* ✅ **6/6 Unit Tests Passing**
* ✅ `JobSubmitRequest` wrapper and nested `Job` messaging verified.
* ✅ `ResourceList` map-based limits (nodes, procs) implemented and tested.
* ✅ `JobStatusUpdate` and state transition logic stable.

---

## 🛠 Prerequisites

To build Torque-NG, you will need:
* **Compiler**: GCC 9+ or Clang 10+ (C++17 support required)
* **Build System**: CMake 3.12+
* **Dependencies**:
    * `protobuf` (3.x+) - Protocol Buffer compiler and runtime
    * `gtest` - Google Test framework
    * `hwloc` (2.0+) - Hardware topology discovery
    * `libssl` / `libcrypto` - Secure communication

---

## 🚀 Quick Start

### Building from Source

```bash
git clone git@github.com:nielsk80/torque-NG.git
cd torque-NG
mkdir build && cd build
cmake ..
make -j$(nproc)