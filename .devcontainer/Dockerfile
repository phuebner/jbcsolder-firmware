FROM ubuntu:24.04

ARG TOOLS_PATH=/opt/gcc-arm-none-eabi
ARG ARM_VERSION=14.2.rel1

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake ninja-build \
    git gnupg2 \
    stlink-tools \
    xz-utils curl

# Get ARM Toolchain
RUN echo "Downloading ARM GNU GCC version ${ARM_VERSION}" \
    && mkdir ${TOOLS_PATH} \
    && curl -Lo gcc-arm-none-eabi.tar.xz "https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_VERSION}/binrel/arm-gnu-toolchain-${ARM_VERSION}-x86_64-arm-none-eabi.tar.xz" \
    && tar xf gcc-arm-none-eabi.tar.xz --strip-components=1 -C ${TOOLS_PATH} \
    && rm gcc-arm-none-eabi.tar.xz \
    && rm ${TOOLS_PATH}/*.txt \
    && rm -rf ${TOOLS_PATH}/share/doc \
    && echo "https://developer.arm.com/-/media/Files/downloads/gnu/${ARM_VERSION}/binrel/arm-gnu-toolchain-${ARM_VERSION}-x86_64-arm-none-eabi.tar.xz"

# Get static analysis tools: clang-format clang-tidy, Lizard, cpplint
RUN apt-get install -y \
    clang-format clang-tidy \
    python3 python3-pip \
    && pip install lizard cpplint --break-system-packages \
    && apt-get clean

# Add Toolchain to PATH
ENV PATH="$PATH:${TOOLS_PATH}/bin"