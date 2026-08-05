#!/bin/bash
set -e

echo "Building cfalse..."
gcc -O3 -Wall -Wextra src/main.c -o cfalse


INSTALL_DIR="/usr/local/bin"

echo "Installing globally to $INSTALL_DIR..."
sudo mkdir -p "$INSTALL_DIR"
sudo cp cfalse "$INSTALL_DIR/cfalse"
sudo chmod +x "$INSTALL_DIR/cfalse"

echo "Installation complete!"
