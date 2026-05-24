FROM ubuntu:24.04 AS builder

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    cmake build-essential git \
    libssl-dev libboost-system-dev libcurl4-openssl-dev \
    tzdata \
    && rm -rf /var/lib/apt/lists/*

RUN git clone --depth=1 https://github.com/reo7sp/tgbot-cpp /tmp/tgbot-cpp \
    && cd /tmp/tgbot-cpp \
    && cmake -DCMAKE_BUILD_TYPE=Release . \
    && make -j"$(nproc)" \
    && make install \
    && rm -rf /tmp/tgbot-cpp

RUN git clone --depth=1 https://github.com/HowardHinnant/date /tmp/date \
    && cd /tmp/date \
    && cmake -DCMAKE_BUILD_TYPE=Release \
             -DUSE_SYSTEM_TZ_DB=ON \
             -DBUILD_TZ_LIB=ON \
             -DENABLE_DATE_TESTING=OFF . \
    && make -j"$(nproc)" \
    && make install \
    && rm -rf /tmp/date

WORKDIR /build
COPY CMakeLists.txt .
COPY src/ src/

RUN cmake -DCMAKE_BUILD_TYPE=Release . && make -j"$(nproc)"

FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    libssl3 libboost-system1.83.0 libcurl4 tzdata ca-certificates \
    && rm -rf /var/lib/apt/lists/*

COPY --from=builder /build/tcz /usr/local/bin/tcz

CMD ["tcz"]
