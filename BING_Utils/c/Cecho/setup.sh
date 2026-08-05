#!/bin/bash
set -e

echo "Building cfalse..."
gcc -O3 -Wall -Wextra src/cecho.c -o cecho


INSTALL_DIR="/usr/local/bin"

echo "Installing globally to $INSTALL_DIR..."
sudo mkdir -p "$INSTALL_DIR"
sudo cp cecho "$INSTALL_DIR/cecho"
sudo chmod +x "$INSTALL_DIR/cecho"

echo "Installation complete!"
