#!/bin/bash
set -e

echo "Building cing..."
gcc -O3 -Wall -Wextra src/cing.c -o cing

PREFIX=${PREFIX:-/usr/local}
DESTDIR=${DESTDIR:-}
INSTALL_DIR="$DESTDIR$PREFIX/bin"

echo "Installing to $INSTALL_DIR..."
mkdir -p "$INSTALL_DIR"
install -m 0755 cing "$INSTALL_DIR/cing"

echo "Installation complete!"
