#!/bin/bash
set -e

echo "Building cpwd..."
gcc -O3 -Wall -Wextra src/cpwd.c -o cpwd


INSTALL_DIR="/usr/local/bin"

echo "Installing globally to $INSTALL_DIR..."
sudo mkdir -p "$INSTALL_DIR"
sudo cp cpwd "$INSTALL_DIR/cpwd"
sudo chmod +x "$INSTALL_DIR/cpwd"

echo "Installation complete!"
