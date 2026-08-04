#!/bin/bash
set -e

echo "Building cfalse..."
gcc -O3 -Wall -Wextra src/main.c -o ctrue


INSTALL_DIR="/usr/local/bin"

echo "Installing globally to $INSTALL_DIR..."
sudo mkdir -p "$INSTALL_DIR"
sudo cp ctrue "$INSTALL_DIR/ctrue"
sudo chmod +x "$INSTALL_DIR/ctrue"

echo "Installation complete!"
