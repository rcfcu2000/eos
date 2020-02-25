FROM ubuntu:18.04 as builder

RUN apt-get update && apt-get install git sudo -y
RUN cd / && git clone -b hub_v2_test https://rcfcu2000:Video0me!@github.com/rcfcu2000/eos.git
RUN cd / && git clone -b hub_test https://rcfcu2000:Video0me!@github.com/rcfcu2000/ncc.git

RUN cd /eos && git submodule update --init --recursive
RUN cd /eos/scripts && chmod +x eosio_build.sh && ./eosio_build.sh -y


FROM ubuntu:18.04

RUN apt-get -y update \
    && apt-get -y dist-upgrade \
    && DEBIAN_FRONTEND=noninteractive apt-get -y install openssl ca-certificates \
    && apt-get -y install wget \
    && wget https://github.com/EOSIO/eosio.cdt/releases/download/v1.5.0/eosio.cdt_1.5.0-1_amd64.deb \
    && apt-get -y install ./eosio.cdt_1.5.0-1_amd64.deb \
    && apt-get -y install nodejs \
    && apt-get -y install npm \
    && apt-get -y install git \
    && apt-get -y install vim \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /usr/local/lib/* /usr/local/lib/
COPY --from=builder /usr/lib/x86_64-linux-gnu/* /usr/local/lib/
COPY --from=builder /lib/x86_64-linux-gnu/libusb* /usr/local/lib/
COPY --from=builder /eos/build/bin /opt/eosio/bin
COPY --from=builder /eos/Docker/nodeosd.sh /opt/eosio/bin/nodeosd.sh

# Copy and buid coinxp contracts
COPY --from=builder /ncc/contracts /coinxp/contracts/
COPY --from=builder /ncc/services/secretmgr /coinxp/services/secretmgr

ENV EOSIO_ROOT=/opt/eosio
RUN chmod +x /opt/eosio/bin/nodeosd.sh
ENV LD_LIBRARY_PATH /usr/local/lib
ENV PATH /opt/eosio/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

# Build coinxp contracts
RUN cd /coinxp/services/secretmgr \
    && npm install \
    && cd /coinxp/contracts/scripts \
    && ./build_contracts.sh \
    && mkdir -p /opt/eosio/bin/data-dir/wallet

