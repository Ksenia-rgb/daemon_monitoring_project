# Daemon monitoring project
Студенческий проект. Система мониторинга, предназначенная для наблюдения за состоянием серверов.

При форке репозитория выберете клонировать только master ветку!

## Правила публикации изменений:
Название PR и сообщение каждого коммита в PR должны содержать идентификатор выполняемой задачи:
- \[client] - клиентская часть
- \[daemon] - демоническая часть
- \[tests] - автоматические тесты (для основного репозитория)
- \[master] - общие файлы, конфигурации (для основного репозитория)

Merge коммиты не оформляются.

## Сборка с помощью Makefile
Все исходные тексты в каждой работе идентифицируются по расширению "cpp". Обнаруженные исходные тексты делятся на группы:
- Исходные тексты работы: все файлы, имена которых не начинаются с "test-".
- Исходные тексты тестов: все файлы, исключая файл "main.cpp".

Поддерживается сборка каждого отдельного компонента - component.

Поддерживаемые цели:

`build-component`: построение компонента, например
```
$ make build-daemon
```

`run-labid`: запуск построенного компонента, например
```
$ make run-daemon
```
Для передачи дополнительных параметров используется переменная `ARGS` (при помощи GNU Make):
```
$ make run-daemon ARGS="daemon/config/config.json"
```

`test-component`: сборка и запуск тестов компонента:
```
$ make test-daemon
```
Переменная TEST_ARGS используется для передачи параметров тестам аналогично `ARGS`.
Если make не находит boost, в корне репозитория нужно создать файл `.boost_location` и указать там путь.

`components`: список всех компонентов в проекте.

Дополнительной возможностью является запуск динамического анализатора Valgrind для запускаемых программ. Для этого необходимо указать в переменной VALGRIND параметры анализатора так, как это делается для `ARGS/TEST_ARGS`:
```
$ make run-daemon ARGS="daemon/config/config.json" VALGRIND="valgrind --leak-check=full"
```

## Автоматическое форматирование
Доступно автоматическое форматирование кода с помощью `clang-format`. Файл с конфигурацией `.clang-format`. Минимальная требуемая версия для использования всех функций - clang-format-20.

Форматирование полностью по CG.

Пример использования:
```
clang-format-20 -i --style=file:.clang-format --files=daemon/src/*.cpp
```

Если не хочется устанавливать новую версию, из файла .clang-format надо будет убрать строки:
- AllowShortNamespacesOnASingleLine: false
- InsertBraces: true
- InsertNewlineAtEOF: true
- KeepEmptyLines:
    AtEndOfFile: true
    AtStartOfBlock: false
- BreakTemplateDeclarations: Yes
- RemoveParentheses: ReturnStatement

Большинство из них некритичные, самое важное `InsertNewlineAtEOF`.

### Установка clang-format-20
В стандартных репозиториях Debian/Ubuntu последняя версия clang-format и других инструментов clang - 14. Чтобы установить более новую версию, нужно использовать скрипт с официального сайта llvm:
```
wget -O – https://apt.llvm.org/llvm.sh”
chmod +x llvm.sh
sudo ./llvm.sh 20
sudo apt install clang-format-20
sudo apt install clang-tidy-20
```

## Docker + Docker Compose (Локальный запуск всей системы)

Для запуска всей системы (storage + несколько daemon'ов + client) используется Docker Compose.

### Требования
- Docker
- Docker Compose v2
- GNU Make

### Быстрый старт

```bash
git clone --branch master https://github.com/Ksenia-rgb/daemon_monitoring_project.git

cd daemon_monitoring_project
```

### 2. Запуск проекта
```bash
make -f docker.mk dev        # первый запуск (сборка образов)
```
или
```bash
make -f docker.mk up         # последующие запуски
```

### Основные команды Docker
```bash
make -f docker.mk help       # Показать все доступные команды
make -f docker.mk dev        # Запуск с пересборкой образов (режим разработки)
make -f docker.mk up         # Запуск без пересборки
make -f docker.mk down       # Остановить все контейнеры
make -f docker.mk logs       # Просмотр логов (всех сервисов)
make -f docker.mk ps         # Статус контейнеров
make -f docker.mk build      # Собрать все образы
make -f docker.mk client     # Запустить клиент
```

### Запуск отдельных частей
#### Только storage
```bash
docker compose up -d storage
```
#### Только один демон
```bash
docker compose up -d daemon-server-a
```
#### Клиент с параметрами
```bash
make client -- --help
#или
docker compose --profile cli run --rm client --help
```

### Добавление нового демона

1) Скопируйте блок daemon-server-b в docker-compose.yml
2) Переименуйте в daemon-server-c
3) Измените SERVER_NAME=server-c

### Структура Docker-части проекта

- docker-compose.yml — описание всех сервисов
- .env — основные настройки (порты, окружение, список серверов)
- Makefile — команды для разработки
- Dockerfile'ы находятся в папках daemon/, storage/, client/
