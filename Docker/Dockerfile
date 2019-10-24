FROM ubuntu:16.04 as builder
ARG eosbranch=hub_exchange
ARG symbol=SYS

# Get coinxp eos source code and compile
RUN git clone -b $eosbranch https://github.com/rcfcu2000/eos.git --recursive \
    &&  cp -f eos/CMakeModules/fc-CMakeLists.txt  eos/libraries/fc/CMakeLists.txt \
    && cd eos && echo "$eosbranch:$(git rev-parse HEAD)" > /etc/eosio-version \
    && cmake -H. -B"/tmp/build" -GNinja -DCMAKE_BUILD_TYPE=Release -DWASM_ROOT=/opt/wasm -DCMAKE_CXX_COMPILER=clang++ \
       -DCMAKE_C_COMPILER=clang -DCMAKE_INSTALL_PREFIX=/tmp/build -DBUILD_MONGO_DB_PLUGIN=true -DCORE_SYMBOL_NAME=$symbol \
    && cmake --build /tmp/build --target install

FROM eosio/builder as ncc
ARG cxpbranch

RUN git clone -b $cxpbranch https://rcfcu2000:Video0me!@github.com/rcfcu2000/ncc.git

FROM ubuntu:16.04

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
COPY --from=builder /tmp/build/bin /opt/eosio/bin
COPY --from=builder /tmp/build/contracts /contracts
COPY --from=builder /eos/Docker/config.ini /
COPY --from=builder /etc/eosio-version /etc
COPY --from=builder /eos/Docker/nodeosd.sh /opt/eosio/bin/nodeosd.sh
# Copy and buid coinxp contracts
COPY --from=ncc /ncc/contracts /coinxp/contracts/
COPY --from=ncc /ncc/services/secretmgr /coinxp/services/secretmgr

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
