// SPDX-FileCopyrightText: AnmiTaliDev <anmitalidev@nuros.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <tgbot/tgbot.h>

#include <cstdint>

void register_handlers(TgBot::Bot& bot, const std::string& bot_username, int32_t start_time);
