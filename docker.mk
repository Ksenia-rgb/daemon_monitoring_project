.PHONY: help up down build rebuild logs ps clean dev prod check-deps

check-deps:
	@echo "Checking dependencies..."
	@which docker >/dev/null 2>&1 || (echo "Docker not found!" && exit 1)
	@docker compose version >/dev/null 2>&1 || (echo "Docker Compose v2 not found" && exit 1)
	@echo "Dependencies: OK"

help: check-deps
	@printf "\nCommands:\n"
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[36m%-20s\033[0m %s\n", $$1, $$2}'

up: check-deps
	docker compose up -d

dev: check-deps
	docker compose up -d --build

prod: check-deps
	ENVIRONMENT=production docker compose up -d

down: check-deps
	docker compose down --remove-orphans

rebuild: check-deps
	docker compose build --no-cache --parallel

logs: check-deps
	docker compose logs -f

ps: check-deps
	docker compose ps

client: check-deps
	docker compose --profile cli run --rm -it client

rebuild-client: check-deps
	docker compose build --no-cache client
