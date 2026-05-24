// SPDX-FileCopyrightText: AnmiTaliDev <anmitalidev@nuros.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "timezone.h"
#include "aliases.h"

#include <algorithm>
#include <cctype>
#include <regex>

static std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return s;
}

std::optional<Timezone> parse_timezone(const std::string& raw) {
    std::string s = raw;
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.front()))) s.erase(s.begin());
    while (!s.empty() && std::isspace(static_cast<unsigned char>(s.back()))) s.pop_back();

    static const std::regex utc_re(R"(UTC\s*([+-]?\d{1,2}))", std::regex::icase);
    std::smatch m;
    if (std::regex_match(s, m, utc_re)) {
        int offset = std::stoi(m[1].str());
        if (offset < -12 || offset > 14)
            return std::nullopt;
        return Timezone{true, offset * 60, nullptr};
    }

    const auto& aliases = get_timezone_aliases();
    auto it = aliases.find(to_lower(s));
    if (it != aliases.end()) {
        try {
            return Timezone{false, 0, date::locate_zone(it->second)};
        } catch (...) {
            return std::nullopt;
        }
    }

    try {
        return Timezone{false, 0, date::locate_zone(s)};
    } catch (...) {
        return std::nullopt;
    }
}

std::chrono::system_clock::time_point make_utc_time(int h, int min, const Timezone& tz) {
    using namespace std::chrono;
    using namespace date;

    auto today_utc = floor<days>(system_clock::now());

    if (tz.is_fixed) {
        auto local_tp = sys_seconds{sys_days{today_utc}}
            + std::chrono::hours{h} + std::chrono::minutes{min};
        return local_tp - std::chrono::minutes{tz.offset_minutes};
    }

    auto zoned_now = make_zoned(tz.zone, system_clock::now());
    auto local_today = floor<days>(zoned_now.get_local_time());
    local_time<seconds> local_tp = local_seconds{local_days{local_today}}
        + std::chrono::hours{h} + std::chrono::minutes{min};

    try {
        return make_zoned(tz.zone, local_tp, choose::earliest).get_sys_time();
    } catch (...) {
        return sys_seconds{sys_days{today_utc}}
            + std::chrono::hours{h} + std::chrono::minutes{min};
    }
}
