#!/bin/bash
# Local package building script

set -e

VERSION=${1:-0.1.0}
BUILD_DIR=${BUILD_DIR:-build}

echo "Building OpenTraceDecode packages v$VERSION"

# Clean and build
rm -rf dist
meson setup $BUILD_DIR -Dprefix=/usr
meson compile -C $BUILD_DIR
meson test -C $BUILD_DIR
meson install -C $BUILD_DIR --destdir dist

# Check if fpm is available
if command -v fpm >/dev/null 2>&1; then
    echo "Building DEB package..."
    fpm -s dir -t deb -n opentracedecode -v "$VERSION" \
      --license GPL-3.0-or-later \
      --maintainer "OpenTraceLab" \
      --url "https://github.com/OpenTraceLab/OpenTraceDecode" \
      --description "OpenTraceDecode: protocol decoding core (Python embedded)" \
      -C dist usr/lib usr/include usr/lib/*/pkgconfig

    echo "Building RPM package..."
    fpm -s dir -t rpm -n opentracedecode -v "$VERSION" \
      --license GPL-3.0-or-later \
      --maintainer "OpenTraceLab" \
      --url "https://github.com/OpenTraceLab/OpenTraceDecode" \
      --description "OpenTraceDecode: protocol decoding core (Python embedded)" \
      -C dist usr/lib usr/include usr/lib64/pkgconfig || true

    echo "Packages built:"
    ls -la *.deb *.rpm 2>/dev/null || echo "No packages found"
else
    echo "fpm not found. Install with: gem install fpm"
    echo "Staged files in dist/ directory"
fi
