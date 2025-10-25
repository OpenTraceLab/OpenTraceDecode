# OpenTraceDecode

OpenTraceDecode is a protocol decoder library providing a shared library for running protocol decoders on signal analysis data. This project focuses on enhanced trace decoding capabilities and modern build systems.

## Versioning

This project follows semantic versioning starting from version 0.1.0. The ABI (Application Binary Interface) is tied to the minor version number, meaning:

- **Major version** (0.x.x): Breaking API/ABI changes
- **Minor version** (x.1.x): ABI changes, new features  
- **Patch version** (x.x.1): Bug fixes, no ABI changes

Current version: **0.1.0**

## About

OpenTraceDecode provides protocol decoder functionality for trace analysis workflows. The protocol decoders are written in Python and provide the foundation for analyzing various communication protocols.

## Building

OpenTraceDecode uses Meson as its build system:

```bash
meson setup builddir
meson compile -C builddir
meson install -C builddir
```

### Developer Options

For development and debugging:

```bash
# Enable LTO, sanitizers, and unity builds
meson setup builddir -Db_lto=true -Db_sanitize=address,undefined -Dunity=on

# Custom decoder path for testing
meson setup builddir -Ddecoders_path=/path/to/custom/decoders

# Build Python extension modules
meson setup builddir -Dpython_shims=true

# Static library build
meson setup builddir -Dbuild_static=true -Dbuild_shared=false
```

## Requirements

- Meson >= 0.60
- Python >= 3.8
- libglib >= 2.34
- pkg-config >= 0.22
- gcc >= 4.0 or clang

## ✅ Multi-Platform CI/CD Pipeline:

### **Platforms Supported:**
• **Linux (Ubuntu):** DEB and RPM packages via fpm
• **macOS:** Native .pkg packages via pkgbuild
• **Windows:** ZIP archives via MSYS2

### **CI Features:**
• **Build & Test:** All platforms compile and run tests
• **Automatic Packaging:** Only on git tags (releases)
• **Artifact Upload:** Packages uploaded for download
• **Dependencies:** Proper installation of build tools and libraries

### **Package Contents:**
• **Libraries:** libopentracedecode.so (shared), .a (static)
• **Headers:** include/opentracedecode/*.h
• **pkg-config:** .pc files for easy integration
• **Proper versioning:** Uses git tag for package version

### **Local Development Tools:**
• **build-packages.sh:** Local package building (requires fpm)
• **prepare-release.sh:** Version bumping and release prep
• **Staging verification:** Install to dist/ directory

### **Release Workflow:**
1. Run ./packaging/prepare-release.sh 1.0.0
2. Commit and tag: git tag v1.0.0 && git push --tags
3. GitHub Actions automatically builds packages for all platforms
4. Download artifacts from GitHub Actions

## License

OpenTraceDecode is licensed under the GNU General Public License (GPL), version 3 or later.

## Community

Join our community discussions on Discord: https://discord.gg/DsYwx59MPh

## Original Project

This project is based on libsigrokdecode from the sigrok project.
