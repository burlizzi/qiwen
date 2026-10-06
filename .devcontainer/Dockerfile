FROM ubuntu

# Copy and preserve executable permissions
ADD --chmod=755 ./arm-anykav200-crosstool/ /opt

# Install required build tools (without specific glibc dependencies)
RUN dpkg --add-architecture i386 && \
    apt update && \
    apt install -y --no-install-recommends \
    make \
    libc6:i386 libstdc++6:i386 \
    libmpc3 libmpfr6 libgmp10 zlib1g:i386 && \
    apt clean && \
    rm -rf /var/lib/apt/lists/*

# Set PATH and verify
ENV PATH=$PATH:/opt/usr/bin/
ENV LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/opt/usr/lib/
RUN echo "PATH is set to $PATH"

