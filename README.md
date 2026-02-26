# Torque-NG (Next Generation)

[![License](https://img.shields.io/badge/License-OpenPBS_2.3-blue.svg)](LICENSE)
[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-orange.svg)](https://en.cppreference.com/w/cpp/17)
[![Platform](https://img.shields.io/badge/Platform-Linux-lightgrey.svg)](https://www.kernel.org/)

**Torque-NG** is a modernized, high-performance successor to the TORQUE Resource Manager. This project brings the legacy codebase into the 2020s, focusing on C++17 standards, container-native isolation, and heterogeneous hardware awareness for x86 and ARM64.

---

## 📖 Overview

TORQUE (Terascale Open-source Resource and Queue Manager) is an open-source project based on the original PBS resource manager developed by NASA, LLNL, and MRJ. Over decades, it has incorporated significant advancements in scalability, fault-tolerance, and security from a global community of contributors.

**Torque-NG** continues this legacy by reviving the stagnant upstream repository. While the original project provided a robust foundation, modern HPC requirements—such as hybrid CPU architectures and Cgroup V2—require a fundamental shift in the core architecture. Torque-NG is the evolution of that foundation, refactored for the modern era.



---

## 🌟 Key Modernizations

* **C++17 Architecture**: Replaced legacy C-style utilities (`u_*`) and manual memory management with RAII, standard library containers, and thread-safe patterns.
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
git clone git@github.com:nielsk80/torque-NG.git
cd torque-NG
mkdir build && cd build
cmake ..
make -j$(nproc)
sudo make install