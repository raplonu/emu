

FROM nvidia/cuda:12.3.2-devel-ubi9

RUN dnf install -y \
    # curl wget \
    git \
    gdb \
    python3-devel python3-pip \
    cmake \
    ninja-build \
 && dnf clean all

# nvtop
# fish
# ipythonP
# FROM nvidia/cuda:12.2.0-devel-ubuntu22.04

# [Optional] Uncomment this section to install additional packages.
# RUN apt-get update \
#  && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
#     curl wget \
#     nvtop \
#     git \
#     gdb \
#     python3-dev python3-pip \
#  && rm -rf /var/lib/apt/lists/*

# # coreutils build-essential software-properties-common pkg-config

# RUN add-apt-repository ppa:fish-shell/release-3

# RUN apt-get update \
#  && DEBIAN_FRONTEND=noninteractive apt-get -y install --no-install-recommends \
#     fish \
#  && rm -rf /var/lib/apt/lists/*

# # RUN curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y
# # ENV PATH="/root/.cargo/bin:${PATH}"

# # RUN curl -sS https://starship.rs/install.sh | sh -s -- --yes
# # RUN mkdir -p /root/.config/fish
# # RUN echo "starship init fish | source" >> /root/.config/fish/config.fish

# # RUN cargo install just

RUN python3 -m pip install -U --disable-pip-version-check --no-cache-dir cmake ninja conan ipython

RUN curl -sS https://gitlab.obspm.fr/jbernard/conan-packages/-/raw/main/install.sh | sh
