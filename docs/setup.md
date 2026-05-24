# Setup

## 1. Create a bot via BotFather

1. Open [@BotFather](https://t.me/BotFather)
2. Send `/newbot`, enter a name and username
3. Copy the token

## 2. BotFather settings

Disable Privacy Mode (required to read messages in groups):
```
/setprivacy -> [select bot] -> Disable
```

Enable Inline Mode:
```
/setinline -> [select bot] -> [enter placeholder, e.g.: 13:00 MSK]
```

## 3. Run via Docker

```bash
cp .env.example .env
docker compose up -d --build
docker compose logs -f
```

## 4. Run locally

See [development.md](development.md) for build instructions.

```bash
BOT_TOKEN=your_token ./tcz
```
