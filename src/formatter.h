// SPDX-FileCopyrightText: AnmiTaliDev <anmitalidev@nuros.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>
#include <chrono>
#include "timezone.h"

std::string format_conversion(int h, int min, const std::string& tz_label, const Timezone& tz);
std::string format_current_time();
