# 使用 Ubuntu 22.04 (与你之前的 WSL2 环境一致)
FROM ubuntu:22.04

# 避免交互提示
ENV DEBIAN_FRONTEND=noninteractive

# 1. 安装系统依赖
# 重点新增：
# - libgmp-dev, libmpfr-dev: ISL/Polymer 进行多面体分析的必须依赖
# - ccache: 极大加速 LLVM/MLIR 的增量编译
# - zlib1g-dev: LLVM 依赖
RUN sed -i 's/archive.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list && \
    sed -i 's/security.ubuntu.com/mirrors.aliyun.com/g' /etc/apt/sources.list && \
    apt-get update && apt-get install -y \
    git \
    build-essential \
    cmake \
    ninja-build \
    clang \
    lld \
    python3 \
    python3-pip \
    python3-dev \
    libgmp-dev \
    libmpfr-dev \
    zlib1g-dev \
    libncurses5-dev \
    ccache \
    zsh \
    curl \
    wget \
    sudo \
    gdb \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# 2. 创建用户 'user' (UID 1000)
ARG USERNAME=user
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME -s /bin/zsh \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# 3. 切换用户
USER $USERNAME
WORKDIR /home/$USERNAME

# 4. 配置 Oh-My-Zsh
RUN sh -c "$(curl -fsSL https://raw.githubusercontent.com/ohmyzsh/ohmyzsh/master/tools/install.sh)" "" --unattended

# 5. 安装 Zsh 插件
RUN git clone https://github.com/zsh-users/zsh-autosuggestions ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-autosuggestions && \
    git clone https://github.com/zsh-users/zsh-syntax-highlighting.git ${ZSH_CUSTOM:-~/.oh-my-zsh/custom}/plugins/zsh-syntax-highlighting

# 6. 配置 .zshrc (启用插件 + 配置 ccache)
# 将 /usr/lib/ccache 加入 PATH，这样调用 gcc/clang 时会自动使用 ccache
RUN sed -i 's/plugins=(git)/plugins=(git zsh-autosuggestions zsh-syntax-highlighting)/' ~/.zshrc && \
    echo '\n# Enable ccache\nexport PATH="/usr/lib/ccache:$PATH"' >> ~/.zshrc

CMD ["zsh"]
