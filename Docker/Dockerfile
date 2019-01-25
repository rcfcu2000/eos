FROM eosio/builder as builder
ARG cxpbranch=coinxp/2.0.0
ARG eosbranch=coinxp/1.6.x
ARG symbol=SYS

# Get coinxp eos source code and compile
RUN git clone -b $eosbranch https://github.com/rcfcu2000/eos.git --recursive \
    && cd eos && echo "$eosbranch:$(git rev-parse HEAD)" > /etc/eosio-version \
    && cmake -H. -B"/tmp/build" -GNinja -DCMAKE_BUILD_TYPE=Release -DWASM_ROOT=/opt/wasm -DCMAKE_CXX_COMPILER=clang++ \
       -DCMAKE_C_COMPILER=clang -DCMAKE_INSTALL_PREFIX=/tmp/build -DBUILD_MONGO_DB_PLUGIN=true -DCORE_SYMBOL_NAME=$symbol \
    && cmake --build /tmp/build --target install

# Get coinxp mainchain code
RUN git clone -b $cxpbranch https://rcfcu2000:Video0me!@github.com/rcfcu2000/ncc.git

FROM ubuntu:18.04

RUN apt-get update \
    && DEBIAN_FRONTEND=noninteractive apt-get -y install openssl ca-certificates \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /usr/local/lib/* /usr/local/lib/
COPY --from=builder /tmp/build/bin /opt/eosio/bin
COPY --from=builder /tmp/build/contracts /contracts
COPY --from=builder /eos/Docker/config.ini /
COPY --from=builder /etc/eosio-version /etc
COPY --from=builder /eos/Docker/nodeosd.sh /opt/eosio/bin/nodeosd.sh
# Copy and buid coinxp contracts
COPY --from=builder /ncc/contracts /coinxp/contracts/

ENV EOSIO_ROOT=/opt/eosio
RUN chmod +x /opt/eosio/bin/nodeosd.sh
ENV LD_LIBRARY_PATH /usr/local/lib
ENV PATH /opt/eosio/bin:/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin

# Install eosio.cdt
RUN apt-get update \
    && apt-get install -y wget \
    && rm -rf /var/lib/apt/lists/* \
    && wget https://github.com/EOSIO/eosio.cdt/releases/download/v1.5.0/eosio.cdt_1.5.0-1_amd64.deb \
    && apt-get -y install ./eosio.cdt_1.5.0-1_amd64.deb

# Build coinxp contracts
RUN cd /coinxp/contracts && ./build_contracts.sh

# Additional tools
RUN apt-get update \
    && apt-get -y install vim \
    && rm -rf /var/lib/apt/lists/*\
