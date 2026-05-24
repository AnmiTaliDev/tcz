# tcz — Timezone Converter Bot

Telegram bot for converting time between timezones. 

## Features

- Convert any time to all supported cities at once
- Supports named zones (`Europe/Moscow`, `Asia/Almaty`), aliases (`мск`, `астана`), and offsets (`UTC+5`)
- Show current time on demand
- Inline mode: use the bot in any chat without adding it

## Usage

**Convert time:**
```
13:00 (МСК)
16:30 (UTC+5)
09:00 (Asia/Almaty)
```

**Current time:**
- In private chat — send any message
- In a group — mention the bot `@username` or reply to its message

**Inline:**
```
@bot_username 13:00 МСК
```

## Quick start

```bash
cp .env.example .env
# add BOT_TOKEN to .env
docker compose up -d --build
```

See [docs/setup.md](docs/setup.md) and [docs/development.md](docs/development.md) for details.

## License

GNU General Public License v3.0 — see [LICENSE](LICENSE).
