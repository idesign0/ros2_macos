#!/bin/bash

INPUT_FILE="matched_packages.txt"

if [[ ! -f $INPUT_FILE ]]; then
  echo "File '$INPUT_FILE' not found!"
  exit 1
fi

echo "Starting Homebrew install for packages from $INPUT_FILE..."

while read -r line; do
  # Extract package name and version (version is optional)
  pkg=$(echo "$line" | awk '{print $1}')
  version=$(echo "$line" | awk '{print $2}')

  # Check if package already installed
  if brew list --versions "$pkg" > /dev/null 2>&1; then
    echo "$pkg is already installed."
  else
    # Homebrew does not support installing specific versions by default
    # Just install the latest available version
    echo "Installing $pkg ..."
    brew install "$pkg"
  fi
done < "$INPUT_FILE"

echo "All packages processed."

