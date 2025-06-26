# ROS 2 Humble + Gazebo Harmonic on macOS (Apple Silicon)

This repository provides a streamlined setup for running **ROS 2 Humble** and **Gazebo Sim Harmonic** on **macOS with Apple Silicon (M1/M2/M3)**. ROS 2 is built from source with macOS patches, and Gazebo is installed via Homebrew.

---

## ✅ What's Included

- ROS 2 Humble (built from source) for ARM64, including core packages and important frameworks such as:

  - ament
  - backward_ros
  - eProsima
  - gazebo-release
  - gzsim (including ros2_gz_bridge, gz_ros2_control, and more)
  - osrf
  - ros-drivers (e.g., ackermann_msgs)
  - ros-perception
  - ros-planning (e.g., navigation_msgs)
  - ros-teleop
  - ros-tooling
  - ros-visualization
  - ros
  - ros2
  - ros2_control
  - rviz
 
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

Before proceeding with this repository:

👉 **Follow the official ROS 2 Humble macOS setup guide up to the "Build ROS 2 from Source" step:**

📄 [ROS 2 Humble — macOS Development Setup (docs.ros.org)](https://docs.ros.org/en/humble/Installation/Alternatives/macOS-Development-Setup.html)
> 🔧 Stop at the **"Get the ROS 2 code"** step — return here afterward to continue with this setup.

> ⚠️ **Important Notes:**
> 1) Install ***empy==3.3.4*** to avoid compatibility issues during the ROS 2 build.
> 2) Unlink Homebrew’s Qt (which defaults to version 6). We need version 5 to avoid complications.
> ```bash
> brew unlink qt
> ```
> 3) Use Xcode 16.2 for Building ROS 2 Humble on macOS
>    To ensure a successful build, you **must use Xcode 16.2** specifically. Follow these steps to install and select Xcode 16.2:
>      1. Download Xcode 16.2 from the [Apple Developer website](https://xcodereleases.com).
>      2. Install Xcode 16.2, then select it as the active developer directory by running:
>         
>         ```bash
>         sudo xcode-select -s /Applications/Xcode_2.app/Contents/Developer
>         ```
>         ```bash
>         xcodebuild -version
>         ```
>         
> 5) The steps above install most dependencies, but depending on your macOS setup, some Python packages might still be missing.  
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
eval "$(register-python-argcomplete colcon)"

# Qt5 paths installed via Homebrew
export CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH:$(brew --prefix qt@5)
export PATH=$PATH:$(brew --prefix qt@5)/bin

# Python 3.11 binary path
export PATH="/Library/Frameworks/Python.framework/Versions/3.11/bin:$PATH"

# OpenSSL path for Homebrew
export OPENSSL_ROOT_DIR=/opt/homebrew/opt/openssl@3

# Gazebo version and plugin path (update the path as needed)
export GZ_VERSION=harmonic
# For gz_ros2_control, uncomment after complete build success.
# export GZ_SIM_SYSTEM_PLUGIN_PATH= ~/ros2_ws/install/gz_ros2_control/lib/
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
```

### 2. 🧩 Clone Patched ROS 2 Source (Custom for macOS)

```bash
# Clone This Repository
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src
git clone https://github.com/idesign0/ros2_macos.git .
```
### 3. 🔨 Build from Source
```bash
cd ~/ros2_ws
```
```bash
colcon build --symlink-install \
  --cmake-args -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=Off \
  --packages-ignore qt_gui_cpp rqt_gui_cpp \
  --executor parallel \
  --parallel-workers $(sysctl -n hw.ncpu)
```
What does each option mean?
- `--symlink-install`  
  Uses symlinks for installed files instead of copying — useful for faster iterative development.

- `--cmake-args -DCMAKE_BUILD_TYPE=Release`  
  Builds with **Release** optimizations (faster, optimized binaries). You can switch to `Debug` for debug symbols. *I will keep exploring more ways to optimize the build further and am open to suggestions as well.*

- `-DBUILD_TESTING=Off`  
Disables building test packages. This is recommended because some tests (e.g., in controller_manager) use obsolete C++ methods like random_shuffle, which cause build errors on newer compilers.

- `--packages-ignore qt_gui_cpp rqt_gui_cpp`  
  Skips these two packages known to have macOS issues. See: [ros2/ros2#1139](https://github.com/ros2/ros2/issues/1139)

- `--executor parallel`  
  Runs build tasks in parallel.

- `--parallel-workers $(sysctl -n hw.ncpu)`  
  Sets the number of parallel jobs to your CPU core count (maximizing build speed).
---
## 4. ⚙️ Setup Environment for ROS

After building, source the ROS 2 setup file to overlay your workspace on your shell environment:

```bash
source ~/ros2_ws/install/setup.zsh
```
**Tip**: Add this line to your shell startup file (e.g., ~/.zshrc) to avoid running it every time you open a new terminal.

## 5. ⚠️ Common Errors
Most of the source-related errors are fixed in the macOS-specific source code patches, but there are still some dependency errors which might interrupt a complete build.  
Right now, I have no issues completing the build with more than 423 packages, but as mentioned earlier, it depends on which dependencies are missing on individual Macs.

This section will be constantly updated based on user feedback. Below are some of the most common errors I have faced so far:

### Errors:
1. **ModuleNotFoundError: No module named 'some_library'**  
   For this error, please refer back to the **Important Notes** section above where the installation of missing Python packages using pip is explained in detail.
2. **Case sensitivity issues in Gazebo cmake target files** (e.g., `tinyxml2::tinyxml2` vs `TINYXML2::TINYXML2`)  
   These errors occur due to capitalization mismatches in Homebrew-installed Gazebo cmake files.  
   You will need to manually edit the respective cmake files (e.g., `gz-msgs10-targets.cmake`, `gz-gui8-targets.cmake`) to use the correct lowercase target names.

   - `open /opt/homebrew/Cellar/gz-gui/8.4.0_6/lib/cmake/gz-gui8/gz-gui8-targets.cmake`
   - `open /opt/homebrew/Cellar/gz-msgs10/10.3.2_4/lib/cmake/gz-msgs10/gz-msgs10-targets.cmake`

   Specifically, replace occurrences of `TINYXML2::TINYXML2` with `tinyxml2::tinyxml2` (and similar uppercase target names) to lowercase versions.
  --executor parallel \
  --parallel-workers $(sysctl -n hw.ncpu)
```

