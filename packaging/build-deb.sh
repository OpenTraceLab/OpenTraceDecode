#!/bin/bash
# Simple Debian package build script

set -e

VERSION=$(meson introspect --projectinfo builddir | jq -r '.version')
PACKAGE="libopentracedecode"

echo "Building ${PACKAGE} version ${VERSION}"

# Create build directory
meson setup builddir
meson compile -C builddir

# Install to temporary directory
DESTDIR=$(mktemp -d)
meson install -C builddir --destdir "$DESTDIR"

echo "Package built in $DESTDIR"
echo "Manual packaging required - this is a basic build script"
