// SPDX-FileCopyrightText: AnmiTaliDev <anmitalidev@nuros.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "formatter.h"
#include "aliases.h"

#include <sstream>
#include <iomanip>
#include <date/tz.h>

static std::string format_zoned_time(std::chrono::system_clock::time_point utc_tp,
                                     const std::string& zone_name) {
    using namespace date;
    auto zone = locate_zone(zone_name);
    auto zoned = make_zoned(zone, utc_tp);
    auto local = zoned.get_local_time();
    auto dp = floor<days>(local);
    auto time_of_day = local - dp;
    auto h = floor<std::chrono::hours>(time_of_day);
    auto m = floor<std::chrono::minutes>(time_of_day - h);

    std::ostringstream ss;
    ss << std::setfill('0') << std::setw(2) << h.count()
       << ":" << std::setfill('0') << std::setw(2) << m.count();
    return ss.str();
}

std::string format_conversion(int h, int min, const std::string& tz_label, const Timezone& tz) {
    auto utc_tp = make_utc_time(h, min, tz);

    std::ostringstream out;
    out << "<b>" << std::setfill('0') << std::setw(2) << h
        << ":" << std::setfill('0') << std::setw(2) << min
        << "</b> (<i>" << tz_label << "</i>) — это:\n\n";

    for (const auto& [city, zone_name] : get_cities()) {
        std::string time_str = format_zoned_time(utc_tp, zone_name);
        out << city << "  <code>" << time_str << "</code>\n";
    }

    return out.str();
}

std::string format_current_time() {
    using namespace date;
    using namespace std::chrono;

    auto now = system_clock::now();
    auto utc_zoned = make_zoned("UTC", now);
    auto utc_local = utc_zoned.get_local_time();
    auto dp = floor<days>(utc_local);
    year_month_day ymd{dp};

    std::ostringstream out;
    out << "<b>Текущее время</b>  <i>"
        << std::setfill('0') << std::setw(2) << static_cast<unsigned>(ymd.day())
        << "." << std::setfill('0') << std::setw(2) << static_cast<unsigned>(ymd.month())
        << "." << static_cast<int>(ymd.year())
        << "</i>\n\n";

    for (const auto& [city, zone_name] : get_cities()) {
        std::string time_str = format_zoned_time(now, zone_name);
        out << city << "  <code>" << time_str << "</code>\n";
    }

    return out.str();
}
