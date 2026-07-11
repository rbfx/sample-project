FROM beigirad/tiny-android

# Minimal build tools for Android NDK/CMake builds.
# SDK download, extraction, and submodule initialization happen on the host.
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
        cmake \
        ninja-build \
        python3 && \
    apt-get clean && \
    rm -rf \
        /var/lib/apt/lists/* \
        /var/cache/apt/archives/* \
        /usr/share/doc \
        /usr/share/man \
        /usr/share/locale \
        /usr/share/info

WORKDIR /workspace
