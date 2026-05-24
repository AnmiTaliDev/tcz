# Development

## Dependencies

| Library | Purpose |
|---|---|
| tgbot-cpp | Telegram Bot API |
| Howard Hinnant date | Timezone support (IANA tzdata) |
| OpenSSL | TLS for Telegram API |
| Boost.System | Network I/O (tgbot-cpp dependency) |
| libcurl | HTTP (tgbot-cpp dependency) |

## Build

```bash
sudo apt-get install cmake build-essential libssl-dev \
    libboost-system-dev libcurl4-openssl-dev tzdata

git clone https://github.com/reo7sp/tgbot-cpp
cd tgbot-cpp && cmake . && make -j$(nproc) && sudo make install && cd ..

git clone https://github.com/HowardHinnant/date
cd date && cmake -DUSE_SYSTEM_TZ_DB=ON -DBUILD_TZ_LIB=ON -DENABLE_DATE_TESTING=OFF .
make -j$(nproc) && sudo make install && cd ..

cmake -DCMAKE_BUILD_TYPE=Release .
make -j$(nproc)

BOT_TOKEN=your_token ./tcz
```

## Adding a timezone alias

In `src/aliases.cpp`, inside `get_timezone_aliases()`:
```cpp
{"city_alias", "Region/City"},
```

To add a city to the output, add it to `get_cities()`:
```cpp
{"City Name", "Region/City"},
```

Zone names follow the [IANA tz database](https://en.wikipedia.org/wiki/List_of_tz_database_time_zones).
