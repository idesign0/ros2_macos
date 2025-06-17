# ROS 2 Humble + Gazebo Harmonic on macOS (Apple Silicon)

This repository provides a streamlined setup for running **ROS 2 Humble** and **Gazebo Sim Harmonic** on **macOS with Apple Silicon (M1/M2/M3)**. ROS 2 is built from source with macOS patches, and Gazebo is installed via Homebrew.

---

## ✅ What's Included

- ROS 2 Humble (built from source) for ARM64
- Gazebo Harmonic (installed via Homebrew)
- macOS-specific fixes and configuration
- Clean install process and tested environment

---

## 📦 Prerequisites

This guide will walk you through:

- Setting up Homebrew
- Installing Python 3.11 and core dependencies
- Installing required build tools (`colcon`, `vcstool`, etc.)
- Creating the ROS 2 workspace structure

> 🔧 Stop at the **"Build ROS 2 from source"** step — return here afterward to continue with this setup.

Before proceeding with this repository:

👉 **Follow the official ROS 2 Humble macOS setup guide up to the "Build ROS 2 from Source" step:**

📄 [ROS 2 Humble — macOS Development Setup (docs.ros.org)](https://docs.ros.org/en/humble/Installation/Alternatives/macOS-Development-Setup.html)

> ⚠️ **Important Notes:**
> 1) Install ***empy==3.3.4*** to avoid compatibility issues during the ROS 2 build.
> 2) Unlink Homebrew’s Qt (which defaults to version 6). We need version 5 to avoid complications.
> ```bash
> brew unlink qt
> ```
> 3) The steps above install most dependencies, but depending on your macOS setup, some Python packages might still be missing.  
> If you see errors like:
> 
> ```plaintext
> ModuleNotFoundError: No module named 'some_library'
> ```
> 
> Simply install the missing package with:
> 
> ```bash
> python3.11 -m pip install some_library
> ```
> 
> If you discover any missing dependencies, please **email me** so I can update this guide accordingly! 🙂
> You may need to repeat this step several times for different missing modules.
 
---

## ⚙️ Environment Setup

To avoid exporting environment variables every time you open a terminal, add the following lines to your `~/.zshrc` file:

```bash
# Minimum required CMake version and C++ standard
export CMAKE_POLICY_VERSION_MINIMUM=3.5
export CMAKE_CXX_STANDARD=17

# Qt5 paths installed via Homebrew
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:$(brew --prefix qt@5)
export PATH=$PATH:$(brew --prefix qt@5)/bin

# Python 3.11 binary path
export PATH="/Library/Frameworks/Python.framework/Versions/3.11/bin:$PATH"

# OpenSSL path for Homebrew
export OPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl@3

# Gazebo version and plugin path (update the path as needed)
export GZ_VERSION=harmonic
export GZ_SIM_SYSTEM_PLUGIN_PATH=/Users/dhruvpatel29/humble-ros2/install/gz_ros2_control/lib/
```
After adding these lines, reload your shell with:
```bash
source ~/.zshrc
```
This will ensure your environment variables are set automatically on each new terminal session.

---
## 🛠️ Installation Steps

### 1. Install Gazebo Harmonic

Install Gazebo Harmonic using Homebrew by running:

```bash
brew tap osrf/simulation
brew install gz-harmonic
