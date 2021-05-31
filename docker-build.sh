#! /bin/bash

set -euo pipefail
set +x

export CC="${CC:-clang-11}"
export CXX="${CXX:-clang++-11}"
export SOURCE_DIR="${SOURCE_DIR:-/src}"
export BUILD_DIR="${BUILD_DIR:-docker-build}"


####
# Helpers

function die { echo "ERROR: $*"; exit 1; }


####
# Main

cd "${SOURCE_DIR}" || die "Couldn't cd to ${SOURCE_DIR}"
if [ ! -f "CMakeLists.txt" ]; then die "No CMakeLists in ${pwd}"; fi

# Clear out the build directory.
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"
cd "${BUILD_DIR}" || die "Couldn't cd to ${BUILD_DIR}"

# Configure cmake
cmake -G Ninja .. || die "Cmake generate failed"

# Do the build
cmake --build . || die "Cmake --build failed"
