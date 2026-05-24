// SPDX-FileCopyrightText: AnmiTaliDev <anmitalidev@nuros.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <string>
#include <unordered_map>
#include <vector>
#include <utility>

const std::unordered_map<std::string, std::string>& get_timezone_aliases();
const std::vector<std::pair<std::string, std::string>>& get_cities();
