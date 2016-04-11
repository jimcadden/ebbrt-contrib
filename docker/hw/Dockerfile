FROM ubuntu:trusty

RUN apt-get update -yqq
RUN apt-get install -yqq \
software-properties-common \
git \
wget \
automake \
build-essential \
libtool  \
libfdt-dev \
cmake  \
libboost-dev  \
libboost-filesystem-dev  \
libboost-coroutine-dev  \
libtbb-dev \
dnsmasq 

# GCC 5.x
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt-get update -yqq
RUN apt-get install -yqq g++-5

RUN mkdir -p /tmp

#
# Build and install capnproto
#
RUN wget -O /tmp/capnproto.tar.gz https://github.com/sandstorm-io/capnproto/archive/v0.4.0.tar.gz
WORKDIR /tmp
RUN tar -xf /tmp/capnproto.tar.gz 
WORKDIR /tmp/capnproto-0.4.0/c++
RUN autoreconf -i
RUN ./configure
RUN make -j
RUN make install

# switch to gcc 5.x
RUN ln -f -s /usr/bin/g++-5 /usr/bin/g++

# install EbbRT + toolchain
RUN git clone --recursive https://github.com/sesa/ebbrt.git /tmp/EbbRT
RUN make -C /tmp/EbbRT/baremetal/ext/EbbRT-toolchain
ENV EBBRT_SRCDIR /tmp/EbbRT
RUN make -j -C /tmp/EbbRT/apps/helloworld Debug


