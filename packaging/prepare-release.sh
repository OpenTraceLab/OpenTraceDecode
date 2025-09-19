#!/bin/bash
# Release preparation script

set -e

VERSION=${1:-$(date +%Y.%m.%d)}

echo "Preparing OpenTraceDecode release v$VERSION"

# Update version in meson.build
sed -i "s/version: '[^']*'/version: '$VERSION'/" meson.build

echo "Updated version to $VERSION in meson.build"
echo "Next steps:"
echo "1. git add meson.build"
echo "2. git commit -m 'Release v$VERSION'"
echo "3. git tag v$VERSION"
echo "4. git push origin main --tags"
echo ""
echo "GitHub Actions will automatically build packages for the tag."
