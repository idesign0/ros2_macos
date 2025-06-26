## 🤖 MoveIt 2 on macOS (Apple Silicon)

MoveIt 2 can be built on macOS (Apple Silicon) with a few extra steps.

### ✅ Dependencies

```bash
brew install freeglut ompl
pip install ruckig==0.8.4
```
### 🏗️ Build Instructions
#### 1. Build required perception packages first:
Build the required perception packages from ros-perception first:
```bash
colcon build --base-paths src/ros-perception/ --cmake-clean-cache
```
#### 2. Build OpenMP-dependent MoveIt packages with LLVM
Apple’s default Clang does not support OpenMP. For the following packages, install LLVM and set environment variables before building:
  - moveit_ros_perception
  - moveit_planners_ompl

Set up LLVM and environment variables:

```bash
brew install llvm libomp

export PATH="/opt/homebrew/opt/llvm/bin:$PATH"
export LDFLAGS="-L/opt/homebrew/opt/llvm/lib"
export CPPFLAGS="-I/opt/homebrew/opt/llvm/include"
export CC=/opt/homebrew/opt/llvm/bin/clang
export CXX=/opt/homebrew/opt/llvm/bin/clang++
```
```bash
colcon build --packages-select moveit_ros_perception moveit_planners_ompl --symlink-install \
  --cmake-args -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=Off \
  --executor parallel \
  --parallel-workers $(sysctl -n hw.ncpu)
```
> **Important**: After building these two packages, **restart your terminal** to reset to the default Apple Clang compiler before building the remaining packages.

#### 3. Build the rest of the MoveIt packages:
```bash
colcon build --base-paths src/moveit/ --symlink-install \
  --cmake-args -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=Off \
