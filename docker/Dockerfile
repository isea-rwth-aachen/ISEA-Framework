ARG BASE_IMAGE=base

FROM phusion/baseimage:jammy-1.0.0 AS iseaframe-base

FROM git.isea.rwth-aachen.de:5050/ess/docker/matlab:r2021b AS iseaframe-matlab

FROM iseaframe-${BASE_IMAGE} AS iseaframebase

ARG DEBIAN_FRONTEND=noninteractive


# Set locale environment
ENV LC_ALL=C.UTF-8 \
    LANG=C.UTF-8 \
    LANGUAGE=C.UTF-8

# install base packages
RUN set -ex ; \
    apt-get update ; \
    apt-get -y -qq install --no-install-recommends \
    build-essential \
    cxxtest \
    doxygen \
    g++ \
    gcc \
    git \
    make \
    python3 \
    python3-pip \
    cmake \
    libeigen3-dev \
    libmatio-dev \
    valgrind \
    graphviz ; \
    apt-get -y -qq install --no-install-recommends ccache ; \
    rm -rf /var/lib/apt/lists/*

# install python packages
RUN pip install --no-cache-dir numpy matplotlib scipy


FROM iseaframebase AS iseaframelinux

# noop if base=matlab, needed otherwise
COPY --from=git.isea.rwth-aachen.de:5050/ess/docker/matlab:r2021b /usr/local/MATLAB/R2021b/extern /usr/local/MATLAB/R2021b/extern/
COPY --from=git.isea.rwth-aachen.de:5050/ess/docker/matlab:r2021b /usr/local/MATLAB/R2021b/simulink /usr/local/MATLAB/R2021b/simulink/
COPY --from=git.isea.rwth-aachen.de:5050/ess/docker/matlab:r2021b /usr/local/MATLAB/R2021b/bin/mexext /usr/local/MATLAB/R2021b/bin/
COPY --from=git.isea.rwth-aachen.de:5050/ess/docker/matlab:r2021b /usr/local/MATLAB/R2021b/bin/glnxa64/libmat.so /usr/local/MATLAB/R2021b/bin/glnxa64/
COPY --from=git.isea.rwth-aachen.de:5050/ess/docker/matlab:r2021b /usr/local/MATLAB/R2021b/bin/glnxa64/libmex.so /usr/local/MATLAB/R2021b/bin/glnxa64/
COPY --from=git.isea.rwth-aachen.de:5050/ess/docker/matlab:r2021b /usr/local/MATLAB/R2021b/bin/glnxa64/libmx.so /usr/local/MATLAB/R2021b/bin/glnxa64/

# lbzip2 package in ubuntu hirsute is broken https://bugs.launchpad.net/ubuntu/+source/lbzip2/+bug/1926092
# temporarily installed from focal repo

# install boost
RUN set -ex; \
    cp /etc/apt/sources.list /etc/apt/sources.list.bu ; \
    echo "deb http://cz.archive.ubuntu.com/ubuntu focal main universe" > /etc/apt/sources.list ; \
    apt-get update; \
    apt-get install -y --no-install-recommends \
    wget \
    lbzip2 ; \
    mkdir -p /usr/src/boost; \
    cd /usr/src; \
    wget -O boost.tar.bz2 https://boostorg.jfrog.io/artifactory/main/release/1.67.0/source/boost_1_67_0.tar.bz2; \
    tar -I lbzip2 -xf boost.tar.bz2 -C boost --strip-components=1; \
    cd boost; \
    ./bootstrap.sh; \
    ./b2 install -j 32; \
    cd ..; \
    rm -r boost; \
    rm boost.tar.bz2; \
    apt-get autoremove -y wget lbzip2 ; \
    mv /etc/apt/sources.list.bu /etc/apt/sources.list ; \
    rm -rf /var/lib/apt/lists/*

# force usage of system libraries
RUN rm -f /usr/local/MATLAB/R2021b/sys/os/glnxa64/libstdc++.so.6

FROM iseaframebase AS iseaframecc

RUN set -ex ; \
    apt-get update ; \
    apt-get -y -qq install --no-install-recommends \
    g++-mingw-w64 ; \
    rm -rf /var/lib/apt/lists/* ; \
    cd /usr/lib/ccache ; \
    ln -s $(which ccache) i686-w64-mingw32-gcc-posix ; \
    ln -s $(which ccache) i686-w64-mingw32-g++-posix ; \
    ln -s $(which ccache) i686-w64-mingw32-windres ; \
    ln -s $(which ccache) x86_64-w64-mingw32-g++-posix ; \
    ln -s $(which ccache) x86_64-w64-mingw32-gcc-posix ; \
    ln -s $(which ccache) x86_64-w64-mingw32-windres

# lbzip2 package in ubuntu hirsute is broken https://bugs.launchpad.net/ubuntu/+source/lbzip2/+bug/1926092
# temporarily installed from focal repo

# boost
RUN set -ex ; \
    apt-get update ; \
    DEBIAN_FRONTEND=noninteractive apt-get -y -qq install --no-install-recommends \
    curl p7zip ; \
    curl --fail -L https://downloads.sourceforge.net/project/boost/boost-binaries/1.79.0/boost_1_79_0-bin-msvc-all-32-64.7z -o boost_1_79_0-bin-msvc-all-32-64.7z ; \
    7zr x boost_1_79_0-bin-msvc-all-32-64.7z ; \
    mkdir -p /usr/x86_64-w64-mingw32/FrameworkExtensions/boost ; \
    mkdir -p /usr/x86_64-w64-mingw32/FrameworkExtensions/include ; \
    mkdir -p /usr/x86_64-w64-mingw32/FrameworkExtensions/lib ; \
    mv boost_1_79_0/boost /usr/x86_64-w64-mingw32/FrameworkExtensions/include ; \
    sed -i "s+// #define BOOST_ALL_NO_LIB+#define BOOST_ALL_NO_LIB+g" /usr/x86_64-w64-mingw32/FrameworkExtensions/include/boost/config/user.hpp ; \
    mv boost_1_79_0/lib64-msvc-14.3/* /usr/x86_64-w64-mingw32/FrameworkExtensions/lib ; \
    rm -rf boost_1_79_0-bin-msvc-all-32-64.7z boost_1_79_0 ; \
    apt-get purge --autoremove -y curl p7zip ; \
    rm -rf /var/lib/apt/lists/* ;


# clang/lld/llvm
RUN set -eux; \
    mkdir -p /usr/local/share/keyrings; \
    apt-get update && apt-get install -y -qq --no-install-recommends gpg curl; \
    # clang/lld/llvm
    curl --fail https://apt.llvm.org/llvm-snapshot.gpg.key | gpg --dearmor > /usr/local/share/keyrings/llvm.gpg; \
    echo "deb [signed-by=/usr/local/share/keyrings/llvm.gpg] http://apt.llvm.org/bullseye/ llvm-toolchain-bullseye-13 main" > /etc/apt/sources.list.d/llvm.list; \
    apt-get update && apt-get install --no-install-recommends -y \
    clang-13 \
    llvm-13 \
    lld-13 ; \
    # ensure that clang/clang++ are callable directly
    ln -s clang-13 /usr/bin/clang && ln -s clang /usr/bin/clang++ && ln -s lld-13 /usr/bin/ld.lld; \
    # We also need to setup symlinks ourselves for the MSVC shims because they aren't in the debian packages
    ln -s clang-13 /usr/bin/clang-cl && ln -s llvm-ar-13 /usr/bin/llvm-lib && ln -s lld-link-13 /usr/bin/lld-link; \
    ln -s llvm-mt-13 /usr/bin/mt ; \
    # Verify the symlinks are correct
    clang++ -v; \
    ld.lld -v; \
    # Doesn't have an actual -v/--version flag, but it still exits with 0
    llvm-lib -v; \
    clang-cl -v; \
    lld-link --version; \
    # Use clang instead of gcc when compiling binaries targeting the host (eg proc macros, build files)
    update-alternatives --install /usr/bin/cc cc /usr/bin/clang 100; \
    update-alternatives --install /usr/bin/c++ c++ /usr/bin/clang++ 100; \
    apt-get purge  -y --auto-remove curl gpg; \
    rm -rf /var/lib/apt/lists/*;

# xwin
RUN set -eux; \
    xwin_version="0.2.4"; \
    xwin_prefix="xwin-$xwin_version-x86_64-unknown-linux-musl"; \
    apt-get update && apt-get install -y -qq --no-install-recommends curl tar ; \
    curl --fail -L https://github.com/Jake-Shadle/xwin/releases/download/$xwin_version/$xwin_prefix.tar.gz | tar -xzv --strip-components=1 $xwin_prefix/xwin; \
    # Splat the CRT and SDK files to /xwin/crt and /xwin/sdk respectively
    ./xwin --accept-license splat --output /usr/x86_64-w64-mingw32/FrameworkExtensions/xwin ; \
    ln -s /usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/crt/lib/x86_64/msvcrt.lib /usr/x86_64-w64-mingw32/FrameworkExtensions/xwin/crt/lib/x86_64/msvcrtd.lib ; \
    # Remove unneeded files to reduce image size
    rm -rf .xwin-cache ./xwin; \ 
    apt-get purge  -y --auto-remove curl ; \
    # tar is needed by dpkg
    rm -rf /var/lib/apt/lists/*;

# install zlib
COPY ToolchainWin64.cmake /usr/src/toolchain_files/ToolchainWin64.cmake
COPY ToolchainClangLinux2Win.cmake /usr/src/toolchain_files/ToolchainClangLinux2Win.cmake
RUN cd /usr/src ; \
    git clone https://github.com/madler/zlib.git ; \
    mkdir zlib/build ; \
    cd zlib/build ; \
    cmake .. -DCMAKE_TOOLCHAIN_FILE=/usr/src/toolchain_files/ToolchainClangLinux2Win.cmake ; \
    make -j8 ; \
    mkdir -p /usr/x86_64-w64-mingw32/FrameworkExtensions/lib/64/ ; \
    cp zlibd.dll /usr/x86_64-w64-mingw32/FrameworkExtensions/lib/64/libzlib.dll

# install matio
RUN cd /usr/src ; \
    git clone https://github.com/tbeu/matio.git ; \
    mkdir matio/build ; \
    cd matio/build ; \
    cmake .. -DCMAKE_TOOLCHAIN_FILE=/usr/src/toolchain_files/ToolchainClangLinux2Win.cmake -DMATIO_MAT73:BOOL=OFF -DMATIO_WITH_HDF5:BOOL=OFF -DMATIO_SHARED:BOOL=OFF ; \
    make -j8 ; \
    cp libmatio.lib /usr/x86_64-w64-mingw32/FrameworkExtensions/lib/libmatio64.lib ; \
    mkdir -p /usr/x86_64-w64-mingw32/FrameworkExtensions/include/ ; \
    cp ../src/*.h /usr/x86_64-w64-mingw32/FrameworkExtensions/include/ ; \
    cp ./src/*.h /usr/x86_64-w64-mingw32/FrameworkExtensions/include/

RUN cp /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll /usr/x86_64-w64-mingw32/FrameworkExtensions/lib/64/libwinpthread-1.dll ; \
    cp -r /usr/include/eigen3 /usr/x86_64-w64-mingw32/FrameworkExtensions/include/eigen3

# set up MATLAB for cross-compiling
RUN --mount=type=secret,id=MATLAB_DOWNLOAD_SSH_KEY set -ex; \
    apt-get update; \
    apt-get install -y --no-install-recommends \
    ssh \
    rsync ; \
    cd /usr/x86_64-w64-mingw32; \
    rsync -aHAXxv --numeric-ids --delete --progress -e "ssh -o StrictHostKeyChecking=accept-new -i /run/secrets/MATLAB_DOWNLOAD_SSH_KEY -T -o Compression=no -x" ms630220@login.hpc.itc.rwth-aachen.de:/work/ms630220/windows_cc/Matlab_CC.tar ./matlab.tar ; \
    tar -xf matlab.tar; \
    rm matlab.tar; \
    apt-get autoremove -y ssh rsync ; \
    rm -rf /var/lib/apt/lists/*

# install cxxtest headers
RUN cd /usr/src ; \
    apt-get update; \
    apt-get install -y --no-install-recommends \
    wget ; \
    wget https://netcologne.dl.sourceforge.net/project/cxxtest/cxxtest/4.4/cxxtest-4.4.tar.gz ; \
    tar -xf cxxtest-4.4.tar.gz ; \
    mv cxxtest-4.4/cxxtest/ /usr/x86_64-w64-mingw32/FrameworkExtensions/include/ ; \
    rm cxxtest-4.4.tar.gz ; \
    rm -r cxxtest-4.4 ; \
    apt-get autoremove -y wget

FROM iseaframelinux AS iseaframedev
RUN set -ex ; \
    apt-get update ; \
    apt-get -y install --no-install-recommends \
    liblapack-dev \
    libmpich-dev \
    libopenblas-dev \
    clang \
    clang-format \
    clang-tidy \
    git-lfs ; \
    rm -rf /var/lib/apt/lists/*

FROM iseaframedev AS iseaframeinteractive

COPY set-home-permissions.sh /etc/my_init.d/set-home-permissions.sh

RUN set -ex ; \
    useradd -m -s /bin/bash -G sudo,docker_env isea ; \
    echo "isea:docker" | chpasswd ; \
    echo "isea ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers ; \
    rm /etc/my_init.d/10_syslog-ng.init ; \
    chmod +x /etc/my_init.d/set-home-permissions.sh ; \
    ldconfig

COPY --chown=root:root executables /usr/local/bin/
COPY --chown=isea:isea sFunctions /opt/iseaframe/

USER isea
ENV ISEA_HOME /home/isea
RUN set -ex ; \
    touch $ISEA_HOME/.sudo_as_admin_successful ; \
    mkdir $ISEA_HOME/shared
VOLUME /home/isea/shared



WORKDIR /home/isea
USER root
ENTRYPOINT ["/sbin/my_init","--quiet","--","/sbin/setuser","isea","/bin/bash","-l","-c"]
CMD ["/bin/bash","-i"]
