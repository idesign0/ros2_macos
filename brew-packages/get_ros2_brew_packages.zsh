#!/bin/bash

ROS2_SRC="/Users/dhruvpatel29/humble-ros2/src"

echo "Extracting find_package names from ROS2 source..."
grep -rho 'find_package([^)]*)' "$ROS2_SRC" | sed 's/find_package(//; s/)//' | sort -u > found_packages.txt
echo "Found $(wc -l < found_packages.txt) unique packages."

echo "Getting installed Homebrew formula packages with versions..."
brew list --formula --versions > brew_packages_versions.txt
echo "Homebrew packages list saved."

echo "Matching ROS2 packages with Homebrew packages..."
grep -Ff found_packages.txt brew_packages_versions.txt > matched_packages.txt
echo "Matched packages saved to matched_packages.txt."
