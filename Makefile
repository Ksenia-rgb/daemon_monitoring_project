
BOOST_LOCATION := $(shell test -f .boost_location && cat .boost_location ; true)

CPPFLAGS += -Wall -Wextra -Werror -Wno-missing-field-initializers -Werror=vla -Wold-style-cast $(if $(BOOST_LOCATION),-isystem $(BOOST_LOCATION))
CXXFLAGS += -g -lncurses

TARGET = app
SOURCES = src/client/main.cpp src/client/CLI.cpp
INCLUDE = /home/faxryzen/daemon_monitoring_project/include/daemon_monitoring_project/client

ifneq 'MINGW' '$(patsubst MINGW%,MINGW,$(system))'
CPPFLAGS += -std=c++17
else
CPPFLAGS += -std=gnu++17
endif

all: $(TARGET)

$(TARGET): $(SOURCES)
	g++ $^ $(CPPFLAGS) $(CXXFLAGS) -I $(INCLUDE) -o $@
