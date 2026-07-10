FROM mobiledevops/android-sdk-image:36.1.0

# Remove Java 21 (incompatible with Gradle 7.3.3), install Java 11, CMake, Ninja, Git
RUN apt-get update && \
    apt-get remove -y openjdk-21-jdk openjdk-21-jre && \
    apt-get install -y openjdk-11-jdk curl unzip git cmake ninja-build && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/* && \
    update-alternatives --set java /usr/lib/jvm/java-11-openjdk-amd64/bin/java && \
    update-alternatives --set javac /usr/lib/jvm/java-11-openjdk-amd64/bin/javac

ENV JAVA_HOME=/usr/lib/jvm/java-11-openjdk-amd64

# Set work directory
WORKDIR /workspace