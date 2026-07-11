FROM beigirad/tiny-android
RUN apt-get update -qq && apt-get install -y --no-install-recommends \
    cmake ninja-build && apt-get clean && rm -rf /var/lib/apt/lists/*
WORKDIR /workspace
