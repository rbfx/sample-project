FROM beigirad/tiny-android
RUN apt-get update -qq && apt-get install -y --no-install-recommends \
    cmake ninja-build openjdk-11-jdk wget unzip python3 && apt-get clean && rm -rf /var/lib/apt/lists/*
# Install Gradle 7.5 (compatible with Android Gradle plugin 7.2.1)
RUN wget -q https://services.gradle.org/distributions/gradle-7.5-bin.zip -O /tmp/gradle.zip && \
    unzip -q /tmp/gradle.zip -d /opt && \
    rm /tmp/gradle.zip
ENV GRADLE_HOME=/opt/gradle-7.5
ENV PATH=$PATH:$GRADLE_HOME/bin
WORKDIR /workspace
