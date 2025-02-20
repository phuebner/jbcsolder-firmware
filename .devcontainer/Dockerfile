FROM ubuntu:24.04

ARG TOOLS_PATH=/opt/gcc-arm-none-eabi
ARG ARM_VERSION=14.2.rel1

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake ninja-build \
    git gnupg2 \
    stlink-tools \
    xz-utils curl \
    usbutils vim \
    wget zip unzip tar \
    libsdl2-2.0-0 libsdl2-dev \
    gdb \
    sudo

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

# Install STM32CubeCLT
RUN echo "Downloading libtinfo, libncurses, libaio and glib2, required by stm32cubeclt" \
    && curl -O http://launchpadlibrarian.net/648013231/libtinfo5_6.4-2_amd64.deb && dpkg -i libtinfo5_6.4-2_amd64.deb \
    && curl -O http://launchpadlibrarian.net/648013227/libncurses5_6.4-2_amd64.deb && dpkg -i libncurses5_6.4-2_amd64.deb \
    && curl -O http://launchpadlibrarian.net/646633572/libaio1_0.3.113-4_amd64.deb && dpkg -i libaio1_0.3.113-4_amd64.deb \
    && apt-get install -y glib2.0 \
    && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends tzdata

COPY tools/st-stm32cubeclt_1.17.0_23554_20241124_1810_amd64.sh /install.sh

ENV LICENSE_ALREADY_ACCEPTED=1
RUN chmod +x /install.sh
RUN ./install.sh --quiet /opt/st/stm32cubeclt
ENV STM32CLT_PATH="/opt/st/stm32cubeclt"


# # Create a non-root user to use if preferred -
# # see https://aka.ms/vscode-remote/containers/non-root-user.
# ARG USERNAME=vscode
# ARG USER_UID=1000
# ARG USER_GID=$USER_UID

# # Create the user
# RUN groupadd --gid $USER_GID $USERNAME \
#     && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
#     # [Optional] Add sudo support. Omit if you don't need to install software after connecting.
#     && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
#     && chmod 0440 /etc/sudoers.d/$USERNAME

RUN usermod -aG sudo ubuntu
# New added for disable sudo password
RUN echo '%sudo ALL=(ALL) NOPASSWD:ALL' >> /etc/sudoers

# Switch to the non-root user
USER ubuntu