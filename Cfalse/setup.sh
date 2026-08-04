#!/bin/bash
set -e

echo "Building cfalse..."
gcc -O3 -Wall -Wextra src/main.c -o cfalse

INSTALL_DIR="$HOME/.local/bin"
mkdir -p "$INSTALL_DIR"

cp cfalse "$INSTALL_DIR/cfalse"
chmod +x "$INSTALL_DIR/cfalse"

echo "Installation complete!"

