## ⚠️ Project Status: Experimental Alpha
**Torque-NG is currently in active initial development and is NOT ready for production use.** We are in the process of a "Grand Refactor," transitioning the core libraries from legacy C to C++17. While the foundational `Libutils` now compiles with modern topology and cgroup support, the higher-level daemon logic (MOM/Server) is still being integrated. Use this code for development, testing, and architectural review only.

---

## 📖 Project Overview & Vision

**Torque-NG** (Next Generation) is an ambitious revival of the original TORQUE Resource Manager. TORQUE has historically been a cornerstone of HPC, originating from NASA's PBS project and enhanced by organizations like LLNL, OSC, and NCSA.

However, as hardware has evolved—introducing hybrid P/E-core architectures and ARM-based clusters—and Linux kernel resource management moved to the Unified Cgroup V2 hierarchy, the original codebase became a victim of technical debt. 

### Our Mission
Our goal is to provide a lightweight, modern, and highly-performant alternative to complex schedulers like Slurm or PBS Pro, specifically optimized for:
* **Modern Linux Kernels**: Native, first-class integration with Cgroup V2.
* **Heterogeneous Compute**: Intelligent placement on systems with mixed core efficiencies (Intel Hybrid, ARM big.LITTLE).
* **Developer Productivity**: A clean, C++17 codebase that is easy to audit, extend, and maintain.

# Torque-NG (Next Generation)

[![License](https://img.shields.io/badge/License-TORQUE_v2.5-blue.svg)](LICENSE)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-orange.svg)](https://en.cppreference.com/w/cpp/17)
[![Platform](https://img.shields.io/badge/Platform-Linux-lightgrey.svg)](https://www.kernel.org/)

## ⚠️ Project Status: Experimental Alpha
**Torque-NG is currently in active initial development and is NOT ready for production use.** We are in the process of a "Grand Refactor," transitioning the core libraries from legacy C to C++17. While the foundational `Libutils` now compiles with modern topology and cgroup support, the higher-level daemon logic (MOM/Server) is still being integrated. Use this code for development, testing, and architectural review only.

---

## 📖 Project Overview & Vision

[cite_start]**Torque-NG** (Next Generation) is an ambitious revival of the original TORQUE Resource Manager[cite: 1]. [cite_start]TORQUE has historically been a cornerstone of HPC, originating from the original PBS resource manager developed by NASA, LLNL, and MRJ[cite: 2].

[cite_start]It incorporates significant advancements in scalability, fault-tolerance, and security contributed by organizations such as OSC, NCSA, TeraGrid, the U.S. Dept of Energy, USC, and many others[cite: 3, 4]. However, as hardware has evolved—introducing hybrid P/E-core architectures and ARM-based clusters—and Linux kernel resource management moved to the Unified Cgroup V2 hierarchy, the original codebase became a victim of technical debt. 

### Our Mission
Our goal is to provide a lightweight, modern, and highly-performant alternative to complex schedulers, specifically optimized for:
* **Modern Linux Kernels**: Native, first-class integration with Cgroup V2.
* **Heterogeneous Compute**: Intelligent placement on systems with mixed core efficiencies (Intel Hybrid, ARM big.LITTLE).
* **Developer Productivity**: A clean, C++17 codebase that is easy to audit, extend, and maintain.

---

## 🌟 Key Modernizations

* **C++17 Architecture**: Replaced legacy C-style utilities and manual memory management with RAII, standard library containers, and thread-safe patterns.
* **Heterogeneous Topology Awareness**: Integrated with **hwloc 2.x** to intelligently schedule across **Intel/AMD Hybrid cores (P/E-cores)** and **ARM64 (big.LITTLE/Clusters)**.
* **Linux Cgroup V2**: Native support for the unified control group hierarchy, providing superior resource isolation and management.
* **ARM64 Optimization**: First-class support for ARM-based clusters (AWS Graviton, Ampere Altra), ensuring efficient task placement on complex cache hierarchies.
* **Security & Safety**: Refactored privilege-dropping logic (`UserContext`) using reentrant system calls to eliminate legacy race conditions.

---

## 🛠 Prerequisites

To build Torque-NG, you will need:
* **Compiler**: GCC 9+ or Clang 10+ (C++17 support required)
* **Build System**: CMake 3.12+
* **Libraries**:
    * `hwloc` (2.0+) - Hardware topology discovery
    * `libjsoncpp` - Modern configuration and serialization
    * `libssl` / `libcrypto` - Secure communication

---

## 🚀 Quick Start

### Building from Source

```bash
WARNING! Code is currently under construction. You will be able to clone
the repository but the CMake configuration is currently very premature and
does not yet compile the entir project.

git clone git@github.com:nielsk80/torque-NG.git
cd torque-NG
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install
