# Timezone Converter Bot
# Copyright (C) 2025
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

import os
import re
import hashlib
import telebot
from telebot import types
from dotenv import load_dotenv

load_dotenv()
import pytz
import datetime

# Алиасы часовых поясов
TIMEZONE_ALIASES = {
    # Россия
    "мск": "Europe/Moscow",
    "москва": "Europe/Moscow",
    "питер": "Europe/Moscow",
    "спб": "Europe/Moscow",
    "калининград": "Europe/Kaliningrad",
    "самара": "Europe/Samara",
    "екатеринбург": "Asia/Yekaterinburg",
    "екб": "Asia/Yekaterinburg",
    "омск": "Asia/Omsk",
    "новосибирск": "Asia/Novosibirsk",
    "нск": "Asia/Novosibirsk",
    "сибирь": "Asia/Novosibirsk",
    "красноярск": "Asia/Krasnoyarsk",
    "иркутск": "Asia/Irkutsk",
    "якутск": "Asia/Yakutsk",
    "владивосток": "Asia/Vladivostok",
    "магадан": "Asia/Magadan",
    "камчатка": "Asia/Kamchatka",

    # Украина / Беларусь
    "киев": "Europe/Kiev",
    "украина": "Europe/Kiev",
    "минск": "Europe/Minsk",
    "беларусь": "Europe/Minsk",

    # Кавказ
    "тбилиси": "Asia/Tbilisi",
    "грузия": "Asia/Tbilisi",
    "ереван": "Asia/Yerevan",
    "армения": "Asia/Yerevan",
    "баку": "Asia/Baku",
    "азербайджан": "Asia/Baku",

    # Средняя Азия
    "ташкент": "Asia/Tashkent",
    "узбекистан": "Asia/Tashkent",
    "самарканд": "Asia/Samarkand",
    "нукус": "Asia/Samarkand",
    "бишкек": "Asia/Bishkek",
    "кыргызстан": "Asia/Bishkek",
    "душанбе": "Asia/Dushanbe",
    "таджикистан": "Asia/Dushanbe",
    "ашхабад": "Asia/Ashgabat",
    "туркменистан": "Asia/Ashgabat",

    # Казахстан
    "астана": "Asia/Almaty",
    "алматы": "Asia/Almaty",
    "казахстан": "Asia/Almaty",
    "актау": "Asia/Aqtau",
    "актобе": "Asia/Aqtobe",
    "атырау": "Asia/Atyrau",
    "костанай": "Asia/Qostanay",
    "караганда": "Asia/Almaty",

    # Европа
    "лондон": "Europe/London",
    "англия": "Europe/London",
    "uk": "Europe/London",
    "париж": "Europe/Paris",
    "франция": "Europe/Paris",
    "берлин": "Europe/Berlin",
    "германия": "Europe/Berlin",
    "варшава": "Europe/Warsaw",
    "польша": "Europe/Warsaw",
    "прага": "Europe/Prague",
    "чехия": "Europe/Prague",
    "рим": "Europe/Rome",
    "италия": "Europe/Rome",
    "мадрид": "Europe/Madrid",
    "испания": "Europe/Madrid",
    "амстердам": "Europe/Amsterdam",
    "стамбул": "Europe/Istanbul",
    "турция": "Europe/Istanbul",
    "хельсинки": "Europe/Helsinki",
    "финляндия": "Europe/Helsinki",
    "стокгольм": "Europe/Stockholm",
    "швеция": "Europe/Stockholm",

    # Азия
    "дубай": "Asia/Dubai",
    "оаэ": "Asia/Dubai",
    "токио": "Asia/Tokyo",
    "япония": "Asia/Tokyo",
    "сеул": "Asia/Seoul",
    "корея": "Asia/Seoul",
    "пекин": "Asia/Shanghai",
    "шанхай": "Asia/Shanghai",
    "китай": "Asia/Shanghai",
    "гонконг": "Asia/Hong_Kong",
    "сингапур": "Asia/Singapore",
    "бангкок": "Asia/Bangkok",
    "таиланд": "Asia/Bangkok",
    "джакарта": "Asia/Jakarta",
    "индонезия": "Asia/Jakarta",
    "дели": "Asia/Kolkata",
    "мумбаи": "Asia/Kolkata",
    "индия": "Asia/Kolkata",

    # Америка
    "нью-йорк": "America/New_York",
    "ny": "America/New_York",
    "лос-анджелес": "America/Los_Angeles",
    "la": "America/Los_Angeles",
    "чикаго": "America/Chicago",
    "торонто": "America/Toronto",
    "канада": "America/Toronto",

    # Океания
    "сидней": "Australia/Sydney",
    "австралия": "Australia/Sydney",
    "окленд": "Pacific/Auckland",
    "новая зеландия": "Pacific/Auckland",
}

CITIES = {
    "🇬🇧 Лондон": "Europe/London",
    "🇵🇱 Варшава": "Europe/Warsaw",
    "🇺🇦 Киев / 🇧🇾 Минск": "Europe/Kiev",
    "🇷🇺 Москва": "Europe/Moscow",
    "🇬🇪 Тбилиси / 🇦🇲 Ереван": "Asia/Tbilisi",
    "🇦🇿 Баку": "Asia/Baku",
    "🇺🇿 Ташкент / 🇰🇿 Астана": "Asia/Tashkent",
    "🇰🇿 Актау / Нукус": "Asia/Aqtau",
    "🇷🇺 Новосибирск": "Asia/Novosibirsk",
    "🇷🇺 Владивосток": "Asia/Vladivostok",
}

# Паттерн для поиска времени: 16:10 (UTC+8) или 16:10 (МСК)
TIME_PATTERN = re.compile(
    r'(\d{1,2}):(\d{2})\s*\(([^)]+)\)',
    re.IGNORECASE
)


def parse_timezone(tz_str):
    """Парсит строку часового пояса и возвращает объект timezone."""
    tz_str = tz_str.strip()

    # UTC+X или UTC-X
    utc_match = re.match(r'UTC\s*([+-]?\d{1,2})$', tz_str, re.IGNORECASE)
    if utc_match:
        offset = int(utc_match.group(1))
        if -12 <= offset <= 14:
            return pytz.FixedOffset(offset * 60)
        return None

    # Проверяем алиасы
    tz_lower = tz_str.lower()
    if tz_lower in TIMEZONE_ALIASES:
        return pytz.timezone(TIMEZONE_ALIASES[tz_lower])

    # Пробуем как полное название зоны (Asia/Qostanay)
    try:
        return pytz.timezone(tz_str)
    except pytz.UnknownTimeZoneError:
        return None


def convert_time(hours, minutes, source_tz):
    """Конвертирует время из исходной зоны во все остальные."""
    now = datetime.datetime.now(pytz.UTC)
    naive_time = datetime.datetime(now.year, now.month, now.day, hours, minutes)

    # FixedOffset не имеет localize(), остальные pytz зоны требуют его
    if hasattr(source_tz, 'localize'):
        source_time = source_tz.localize(naive_time)
    else:
        source_time = naive_time.replace(tzinfo=source_tz)

    return source_time


def format_conversion(hours, minutes, tz_str, source_tz):
    """Форматирует результат конвертации времени."""
    source_time = convert_time(hours, minutes, source_tz)

    res = f"<b>{hours:02d}:{minutes:02d}</b> (<i>{tz_str}</i>) — это:\n"
    res += "━━━━━━━━━━━━━━━━━━━━━\n"

    for name, zone in CITIES.items():
        target_tz = pytz.timezone(zone)
        converted = source_time.astimezone(target_tz)
        time_str = converted.strftime('%H:%M')
        res += f"{name}  <code>{time_str}</code>\n"

    return res


def format_current_time():
    """Форматирует текущее время во всех зонах."""
    now_utc = datetime.datetime.now(pytz.UTC)
    date_str = now_utc.strftime('%d.%m.%Y')

    res = f"<b>Текущее время</b>  <i>{date_str}</i>\n"
    res += "━━━━━━━━━━━━━━━━━━━━━\n"

    for name, zone in CITIES.items():
        tz = pytz.timezone(zone)
        local_time = datetime.datetime.now(tz)
        time_str = local_time.strftime('%H:%M')
        res += f"{name}  <code>{time_str}</code>\n"

    return res


HELP_TEXT = """<b>Timezone Converter Bot</b>

<b>Как использовать:</b>

<b>1. Конвертация времени</b>
Напишите время в формате:
<code>13:00 (МСК)</code>
<code>16:30 (UTC+5)</code>
<code>09:00 (Asia/Almaty)</code>

<b>2. Текущее время</b>
В личке — просто напишите что угодно
В группе — упомяните бота или ответьте на его сообщение

<b>3. Inline-режим</b>
В любом чате введите:
<code>@{bot_username} 13:00 МСК</code>

<b>Поддерживаемые зоны:</b>
СНГ: мск, спб, киев, минск, тбилиси, ереван, баку, ташкент, астана, алматы, бишкек, душанбе
Россия: калининград, екб, омск, нск, красноярск, иркутск, владивосток, камчатка
Мир: лондон, париж, берлин, дубай, токио, пекин, сингапур, нью-йорк, сидней
Также: UTC+X, Asia/Almaty и др.

━━━━━━━━━━━━━━━━━━━━━
Разработчики: @AnmiTaliDev & @TriVedRa
GitHub: <a href="https://github.com/AnmiTaliDev/tzc">AnmiTaliDev/tzc</a>
Лицензия: GNU GPL 3.0
Инициатор: @TriVedRa
"""

bot = telebot.TeleBot(os.getenv('BOT_TOKEN'))


@bot.message_handler(commands=['start', 'help'])
def handle_help(message):
    bot_username = bot.get_me().username
    text = HELP_TEXT.format(bot_username=bot_username)
    bot.reply_to(message, text, parse_mode="html")


# Inline-режим: паттерн для "13:00 МСК" или "13:00 UTC+5"
INLINE_PATTERN = re.compile(
    r'(\d{1,2}):(\d{2})\s+(.+)',
    re.IGNORECASE
)


@bot.inline_handler(lambda query: len(query.query) >= 3)
def handle_inline(query):
    results = []

    match = INLINE_PATTERN.match(query.query.strip())
    if match:
        hours = int(match.group(1))
        minutes = int(match.group(2))
        tz_str = match.group(3).strip()

        if hours <= 23 and minutes <= 59:
            source_tz = parse_timezone(tz_str)
            if source_tz:
                text = format_conversion(hours, minutes, tz_str, source_tz)
                result_id = hashlib.md5(query.query.encode()).hexdigest()

                results.append(types.InlineQueryResultArticle(
                    id=result_id,
                    title=f"Конвертировать {hours:02d}:{minutes:02d} ({tz_str})",
                    description="Нажмите, чтобы отправить",
                    input_message_content=types.InputTextMessageContent(
                        message_text=text,
                        parse_mode="HTML"
                    )
                ))

    if not results:
        results.append(types.InlineQueryResultArticle(
            id="help",
            title="Формат: 13:00 МСК",
            description="Например: 13:00 МСК, 16:30 UTC+5, 09:00 астана",
            input_message_content=types.InputTextMessageContent(
                message_text="Используйте формат: <code>13:00 (МСК)</code>",
                parse_mode="HTML"
            )
        ))

    bot.answer_inline_query(query.id, results, cache_time=60)


@bot.message_handler(func=lambda message: True)
def handle_interaction(message):
    if not message.text:
        return

    bot_username = bot.get_me().username

    # Проверяем наличие времени в формате "16:10 (UTC+8)" или "16:10 (МСК)"
    time_match = TIME_PATTERN.search(message.text)

    if time_match:
        hours = int(time_match.group(1))
        minutes = int(time_match.group(2))
        tz_str = time_match.group(3)

        if hours > 23 or minutes > 59:
            return

        source_tz = parse_timezone(tz_str)
        if not source_tz:
            bot.reply_to(
                message,
                f"Не удалось распознать часовой пояс: <code>{tz_str}</code>",
                parse_mode="html"
            )
            return

        res = format_conversion(hours, minutes, tz_str, source_tz)
        bot.reply_to(message, res, parse_mode="html")
        return

    # Обычный режим — показать текущее время
    is_mentioned = f"@{bot_username}" in message.text
    is_reply_to_bot = (
        message.reply_to_message and
        message.reply_to_message.from_user.username == bot_username
    )

    if is_mentioned or is_reply_to_bot or message.chat.type == 'private':
        res = format_current_time()
        bot.reply_to(message, res, parse_mode="html")


if __name__ == '__main__':
    print('Бот запущен')
    bot.infinity_polling()
