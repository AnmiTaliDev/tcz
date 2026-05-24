// SPDX-FileCopyrightText: AnmiTaliDev <anmitalidev@nuros.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "handlers.h"

#include <cstdlib>
#include <ctime>
#include <iostream>
#include <tgbot/tgbot.h>

static void drain_updates(TgBot::Bot& bot) {
    try {
        auto updates = bot.getApi().getUpdates(0, 100, 0);
        while (!updates.empty()) {
            int32_t last_id = updates.back()->updateId;
            updates = bot.getApi().getUpdates(last_id + 1, 100, 0);
        }
    } catch (...) {}
}

int main() {
    const char* token_env = std::getenv("BOT_TOKEN");
    if (!token_env || std::string(token_env).empty()) {
        std::cerr << "BOT_TOKEN is not set\n";
        return 1;
    }

    TgBot::Bot bot(token_env);
    std::string bot_username = bot.getApi().getMe()->username;

    drain_updates(bot);
    int32_t start_time = static_cast<int32_t>(std::time(nullptr));
    register_handlers(bot, bot_username, start_time);

    std::cout << "Bot started: @" << bot_username << "\n";

    TgBot::TgLongPoll long_poll(bot);
    while (true) {
        try {
            long_poll.start();
        } catch (const TgBot::TgException& e) {
            std::cerr << "TgBot error: " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << "\n";
        }
    }
}
