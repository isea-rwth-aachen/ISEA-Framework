ARG BASE_IMAGE=base

FROM phusion/baseimage:hirsute AS iseaframe-base

FROM mathworks/matlab:r2020b AS iseaframe-matlab

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
    libarmadillo-dev \
    valgrind \
    graphviz ; \
    apt-get -y -qq install --no-install-recommends ccache ; \
    rm -rf /var/lib/apt/lists/*

# install python packages
RUN pip install --no-cache-dir numpy matplotlib scipy

FROM iseaframebase AS iseaframelinux

# noop if base=matlab, needed otherwise
COPY --from=iseaframe-matlab /opt/matlab/R2020b/appdata/version.xml /usr/local/MATLAB/R2020b/appdata/version.xml
COPY --from=iseaframe-matlab /opt/matlab/R2020b/extern /usr/local/MATLAB/R2020b/extern/
COPY --from=iseaframe-matlab /opt/matlab/R2020b/bin/mexext /usr/local/MATLAB/R2020b/bin/
COPY --from=iseaframe-matlab /opt/matlab/R2020b/bin/glnxa64/libmat.so /usr/local/MATLAB/R2020b/bin/glnxa64/
COPY --from=iseaframe-matlab /opt/matlab/R2020b/bin/glnxa64/libmex.so /usr/local/MATLAB/R2020b/bin/glnxa64/
COPY --from=iseaframe-matlab /opt/matlab/R2020b/bin/glnxa64/libmx.so /usr/local/MATLAB/R2020b/bin/glnxa64/

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

# install boost
COPY patches/boost/gcc.jam.patch /usr/src/patches/boost/gcc.jam.patch
COPY patches/boost/mc.jam.patch /usr/src/patches/boost/mc.jam.patch
RUN set -ex; \
    cp /etc/apt/sources.list /etc/apt/sources.list.bu ; \
    echo "deb http://cz.archive.ubuntu.com/ubuntu focal main universe" > /etc/apt/sources.list ; \
    apt-get update; \
    apt-get install -y --no-install-recommends \
      wget \
      patch \
      lbzip2 ; \
    mkdir -p /usr/src/boost; \
    cd /usr/src; \
    wget -O boost.tar.bz2 https://boostorg.jfrog.io/artifactory/main/release/1.71.0/source/boost_1_71_0.tar.bz2; \
    tar -I lbzip2 -xf boost.tar.bz2 -C boost --strip-components=1; \
    cd boost; \
    patch /usr/src/boost/tools/build/src/tools/gcc.jam /usr/src/patches/boost/gcc.jam.patch ; \
    patch /usr/src/boost/tools/build/src/tools/mc.jam /usr/src/patches/boost/mc.jam.patch ; \
    echo "using gcc : mingw : x86_64-w64-mingw32-g++-posix ;" > user-config.jam; \
    ./bootstrap.sh mingw; \
    ./b2 --user-config=./user-config.jam toolset=gcc-mingw mc-compiler=windmc target-os=windows variant=release address-model=64 stage -j $(nproc) || :; \
    mkdir -p /usr/x86_64-w64-mingw32/FrameworkExtensions/boost; \
    mkdir -p /usr/x86_64-w64-mingw32/FrameworkExtensions/include; \
    mkdir -p /usr/x86_64-w64-mingw32/FrameworkExtensions/lib; \
    cp -R /usr/src/boost/boost /usr/x86_64-w64-mingw32/FrameworkExtensions/include/boost; \
    cp stage/lib/* /usr/x86_64-w64-mingw32/FrameworkExtensions/lib/; \
    cd ..; \
    rm -r patches; \
    rm -r boost; \
    rm boost.tar.bz2; \
    apt-get autoremove -y wget patch lbzip2 ; \
    mv /etc/apt/sources.list.bu /etc/apt/sources.list ; \
    rm -rf /var/lib/apt/lists/*
    
# install zlib
COPY ToolchainWin64.cmake /usr/src/toolchain_files/ToolchainWin64.cmake
RUN cd /usr/src ; \
    git clone https://github.com/madler/zlib.git ; \
    mkdir zlib/build ; \
    cd zlib/build ; \
    cmake .. -DCMAKE_TOOLCHAIN_FILE=/usr/src/toolchain_files/ToolchainWin64.cmake ; \
    make -j8 ; \
    make install ; \
    mkdir -p /usr/x86_64-w64-mingw32/FrameworkExtensions/lib/64/ ; \
    cp libzlib.dll /usr/x86_64-w64-mingw32/FrameworkExtensions/lib/64/
    
# install matio
RUN cd /usr/src ; \
    git clone https://github.com/tbeu/matio.git ; \
    mkdir matio/build ; \
    cd matio/build ; \
    cmake .. -DCMAKE_TOOLCHAIN_FILE=/usr/src/toolchain_files/ToolchainWin64.cmake -DMATIO_MAT73:BOOL=OFF -DMATIO_WITH_HDF5:BOOL=OFF -DMATIO_SHARED:BOOL=OFF ; \
    make -j8 ; \
    make install ; \
    cp libmatio.a /usr/x86_64-w64-mingw32/FrameworkExtensions/lib/libmatio64.lib ; \
    mkdir -p /usr/x86_64-w64-mingw32/FrameworkExtensions/include/ ; \
    cp ../src/*.h /usr/x86_64-w64-mingw32/FrameworkExtensions/include/ ; \
    cp ./src/*.h /usr/x86_64-w64-mingw32/FrameworkExtensions/include/

RUN cp /usr/x86_64-w64-mingw32/lib/libwinpthread-1.dll /usr/x86_64-w64-mingw32/FrameworkExtensions/lib/64/libwinpthread-1.dll ; \
    cp -r /usr/include/eigen3 /usr/x86_64-w64-mingw32/FrameworkExtensions/include/eigen3 ; \
    cp -r /usr/include/armadillo_bits /usr/x86_64-w64-mingw32/FrameworkExtensions/include/armadillo_bits ; \
    cp /usr/include/armadillo /usr/x86_64-w64-mingw32/FrameworkExtensions/include/armadillo

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
