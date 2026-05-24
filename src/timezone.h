// SPDX-FileCopyrightText: AnmiTaliDev <anmitalidev@nuros.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>
#include <optional>
#include <chrono>
#include <date/tz.h>

struct Timezone {
    bool is_fixed;
    int offset_minutes;
    const date::time_zone* zone;
};

std::optional<Timezone> parse_timezone(const std::string& tz_str);

// Returns UTC time point representing the given local H:M in the source timezone
std::chrono::system_clock::time_point make_utc_time(int h, int min, const Timezone& tz);
