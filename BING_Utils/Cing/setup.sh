#!/bin/bash
set -e

echo "Building cfalse..."
gcc -O3 -Wall -Wextra src/main.c -o cing


INSTALL_DIR="/usr/local/bin"

echo "Installing globally to $INSTALL_DIR..."
sudo mkdir -p "$INSTALL_DIR"
sudo cp cing "$INSTALL_DIR/cing"
sudo chmod +x "$INSTALL_DIR/cing"

echo "Installation complete!"
