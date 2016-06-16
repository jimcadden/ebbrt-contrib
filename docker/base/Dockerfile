FROM ubuntu:xenial
RUN apt-get update && \
    apt-get install -y \
    autoconf \
    automake \
    build-essential \
    git \
    libfdt-dev \
    libglib2.0-dev \
    libpixman-1-dev \
    zlib1g-dev \
    dnsmasq 

RUN mkdir -p /tmp
# QEMU BUILD
RUN git clone -b pin-threads https://github.com/SESA/qemu.git /tmp/qemu
WORKDIR /tmp/qemu
RUN ./configure --target-list=x86_64-softmmu --enable-vhost-net --enable-kvm
RUN make -j
RUN make install
RUN make distclean
# SET ENTRYPOINT
COPY startvm /tmp/startvm
ENTRYPOINT ["/tmp/startvm"]
CMD []
