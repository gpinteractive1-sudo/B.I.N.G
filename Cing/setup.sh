#!/bin/bash
set -e

echo "Building cing..."
gcc -O3 -Wall -Wextra src/main.c -o cing

INSTALL_DIR="$HOME/.local/bin"
mkdir -p "$INSTALL_DIR"

cp cing "$INSTALL_DIR/cing"
chmod +x "$INSTALL_DIR/cing"

echo "Installation complete!"
echo "You can now run 'cing' from anywhere."