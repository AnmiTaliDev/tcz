// SPDX-FileCopyrightText: AnmiTaliDev <anmitalidev@nuros.org>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "handlers.h"
#include "timezone.h"
#include "formatter.h"

#include <regex>
#include <functional>
#include <string>

static const std::string HELP_TEXT =
    "<b>Timezone Converter Bot</b>\n\n"
    "<b>Конвертация времени:</b>\n"
    "<code>13:00 (МСК)</code>\n"
    "<code>16:30 (UTC+5)</code>\n"
    "<code>09:00 (Asia/Almaty)</code>\n\n"
    "<b>Текущее время:</b>\n"
    "В личке — напишите что угодно\n"
    "В группе — упомяните бота или ответьте на его сообщение\n\n"
    "<b>Inline-режим:</b>\n"
    "<code>@bot_username 13:00 МСК</code>\n\n"
    "<b>Разработчики:</b> @AnmiTaliDev &amp; @TriVedRa\n"
    "<b>Лицензия:</b> GNU GPL 3.0";

static const std::regex TIME_PATTERN(
    R"((\d{1,2}):(\d{2})\s*\(([^)]+)\))",
    std::regex::icase
);

static const std::regex INLINE_PATTERN(
    R"((\d{1,2}):(\d{2})\s+(.+))",
    std::regex::icase
);

static void send_reply(TgBot::Bot& bot, TgBot::Message::Ptr msg, const std::string& text) {
    try {
        bot.getApi().sendMessage(msg->chat->id, text, nullptr, nullptr, nullptr, "HTML");
    } catch (const std::exception& e) {
        std::cerr << "send error [" << msg->chat->id << "]: " << e.what() << "\n";
    }
}

void register_handlers(TgBot::Bot& bot, const std::string& bot_username, int32_t start_time) {
    bot.getEvents().onCommand("start", [&bot, start_time](TgBot::Message::Ptr msg) {
        if (msg->date < static_cast<uint32_t>(start_time)) return;
        send_reply(bot, msg, HELP_TEXT);
    });

    bot.getEvents().onCommand("help", [&bot, start_time](TgBot::Message::Ptr msg) {
        if (msg->date < static_cast<uint32_t>(start_time)) return;
        send_reply(bot, msg, HELP_TEXT);
    });

    bot.getEvents().onInlineQuery([&bot](TgBot::InlineQuery::Ptr query) {
        std::vector<TgBot::InlineQueryResult::Ptr> results;
        std::smatch m;

        if (std::regex_search(query->query, m, INLINE_PATTERN)) {
            int h = std::stoi(m[1].str());
            int min = std::stoi(m[2].str());
            std::string tz_str = m[3].str();

            if (h <= 23 && min <= 59) {
                auto tz = parse_timezone(tz_str);
                if (tz) {
                    std::string text = format_conversion(h, min, tz_str, *tz);
                    std::string id = std::to_string(std::hash<std::string>{}(query->query));

                    auto content = std::make_shared<TgBot::InputTextMessageContent>();
                    content->messageText = text;
                    content->parseMode = "HTML";

                    auto result = std::make_shared<TgBot::InlineQueryResultArticle>();
                    result->id = id;
                    result->title = "Convert " + m[1].str() + ":" + m[2].str() + " (" + tz_str + ")";
                    result->description = "Tap to send";
                    result->inputMessageContent = content;
                    results.push_back(result);
                }
            }
        }

        if (results.empty()) {
            auto content = std::make_shared<TgBot::InputTextMessageContent>();
            content->messageText = "Format: <code>13:00 (МСК)</code>";
            content->parseMode = "HTML";

            auto result = std::make_shared<TgBot::InlineQueryResultArticle>();
            result->id = "help";
            result->title = "Format: 13:00 MSK";
            result->description = "Examples: 13:00 МСК, 16:30 UTC+5, 09:00 астана";
            result->inputMessageContent = content;
            results.push_back(result);
        }

        try {
            bot.getApi().answerInlineQuery(query->id, results, 60);
        } catch (const std::exception& e) {
            std::cerr << "answerInlineQuery failed: " << e.what() << "\n";
        }
    });

    bot.getEvents().onAnyMessage([&bot, bot_username, start_time](TgBot::Message::Ptr msg) {
        if (msg->date < static_cast<uint32_t>(start_time)) return;
        if (msg->text.empty()) return;
        if (msg->text.front() == '/') return;

        std::smatch m;
        if (std::regex_search(msg->text, m, TIME_PATTERN)) {
            int h = std::stoi(m[1].str());
            int min = std::stoi(m[2].str());
            std::string tz_str = m[3].str();

            if (h > 23 || min > 59) return;

            auto tz = parse_timezone(tz_str);
            if (!tz) {
                send_reply(bot, msg,
                    "Не удалось распознать часовой пояс: <code>" + tz_str + "</code>");
                return;
            }

            send_reply(bot, msg, format_conversion(h, min, tz_str, *tz));
            return;
        }

        bool is_private = msg->chat->type == TgBot::Chat::Type::Private;
        bool is_mentioned = msg->text.find("@" + bot_username) != std::string::npos;
        bool is_reply_to_bot = msg->replyToMessage &&
                               msg->replyToMessage->from &&
                               msg->replyToMessage->from->username == bot_username;

        if (is_private || is_mentioned || is_reply_to_bot) {
            send_reply(bot, msg, format_current_time());
        }
    });
}
