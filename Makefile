.PHONY: all labs clean all-dockers
.SECONDEXPANSION:
.SECONDARY:

SRC_DIR 		:= src
INCLUDE_DIR := include
TEST_DIR 		:= tests
LIBS_DIR 		:= libs

BOOST_LOCATION := $(shell test -f .boost_location && cat .boost_location ; true)

ifneq 'yes' '$(VERBOSE)'
hidecmd := @
endif
# The variable SILENT controls additional messages

CPPFLAGS += -Wall -Wextra -Werror -Wno-missing-field-initializers -Werror=vla -Wold-style-cast $(if $(BOOST_LOCATION),-isystem $(BOOST_LOCATION))
CXXFLAGS += -g

system   := $(shell uname)

ifneq 'MINGW' '$(patsubst MINGW%,MINGW,$(system))'
CPPFLAGS += -std=c++17
else
CPPFLAGS += -std=gnu++17
endif

ifeq 'Darwin' '$(system)'
TIMEOUT_CMD := gtimeout
else
TIMEOUT_CMD := timeout
endif

components 		:= client daemon storage

sources        = $(wildcard $(1)/$(SRC_DIR)/*.cpp)
headers        = $(wildcard $(1)/$(INCLUDE_DIR)/*.h) $(wildcard $(1)/$(INCLUDE_DIR)/*.hpp)
test_sources   = $(wildcard $(1)/$(TEST_DIR)/test-*.cpp)
libs					 = $(wildcard $(LIBS_DIR)/*.h) $(wildcard $(LIBS_DIR)/*.hpp)

objects			   = $(patsubst %.cpp,out/$(1)/%.o,$(notdir $(call sources,$(1))))
test_objects   = $(patsubst %.cpp,out/$(1)/%.o,$(notdir $(call test_sources,$(1))))
header_checks  = $(addprefix out/$(1)/,$(addsuffix .header,$(notdir $(call headers,$(1)))))

all: $(addprefix build-,$(components))

components:
	@echo $(components)

clean:
	rm -rf out

$(addprefix build-,$(components)): build-%: out/%/component

$(addprefix run-,$(components)): run-%: out/%/component
	@$(FAULT_INJECTION_CONFIG) $(if $(TIMEOUT),$(TIMEOUT_CMD) --signal=KILL $(TIMEOUT)s )$(if $(VALGRIND),valgrind $(VALGRIND) )$< $(ARGS)

$(addprefix test-,$(components)): test-%: out/%/test-component
	$(if $(SILENT),,@echo [TEST] $(patsubst out/test/%,%,$<))
	$(hidecmd)$(if $(TIMEOUT),$(TIMEOUT_CMD) --signal=KILL $(TIMEOUT)s )$(if $(VALGRIND),valgrind $(VALGRIND) )$< $(TEST_ARGS)

out/client/component: $$(call objects,client) $$(call header_checks,client) | $$(@D)/.dir
	$(if $(SILENT),,@echo [LINK] client)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -Iclient/include -o $@ $(filter-out daemon.header,$^)

out/daemon/component: $$(call objects,daemon) $$(call header_checks,daemon) | $$(@D)/.dir
	$(if $(SILENT),,@echo [LINK] daemon)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -Idaemon/include -o $@ $(filter-out daemon.header,$^)

out/storage/component: $$(call objects,storage) $$(call header_checks,storage) | $$(@D)/.dir
	$(if $(SILENT),,@echo [LINK] storage)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -Istorage/include -o $@ $(filter-out daemon.header,$^)


out/client/test-component: $$(call test_objects,client) $$(call objects,client) | $$(@D)/.dir
	$(if $(SILENT),,@echo [LINK] test-client)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -Iclient/include -o $@ $(filter-out %/main.o,$^)

out/daemon/test-component: $$(call test_objects,daemon) $$(call objects,daemon) | $$(@D)/.dir
	$(if $(SILENT),,@echo [LINK] test-daemon)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -Idaemon/include -o $@ $(filter-out %/main.o,$^)

out/storage/test-component: $$(call test_objects,storage) $$(call objects,storage) | $$(@D)/.dir
	$(if $(SILENT),,@echo [LINK] test-storage)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -Istorage/include -o $@ $(filter-out %/main.o,$^)


out/client/%.o: client/src/%.cpp
	@mkdir -p $(dir $@)
	$(if $(SILENT),,@echo [C++ ] $<)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Iclient/include -Ilibs -MMD -MP -c -o $@ $<

out/client/%.hpp.header: client/include/%.hpp
	@mkdir -p $(dir $@)
	$(if $(SILENT),,@echo [HDR ] $<)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Ilibs -c -fsyntax-only $<
	@touch $@

out/client/%.o: client/tests/%.cpp
	@mkdir -p $(dir $@)
	$(if $(SILENT),,@echo [C++ ] $<)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Idaemon/include -Ilibs -Wno-old-style-cast -Wno-unused-parameter -MMD -MP -c -o $@ $<


out/daemon/%.o: daemon/src/%.cpp
	@mkdir -p $(dir $@)
	$(if $(SILENT),,@echo [C++ ] $<)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Idaemon/include -Ilibs -MMD -MP -c -o $@ $<

out/daemon/%.hpp.header: daemon/include/%.hpp
	@mkdir -p $(dir $@)
	$(if $(SILENT),,@echo [HDR ] $<)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Ilibs -c -fsyntax-only $<
	@touch $@

out/daemon/%.o: daemon/tests/%.cpp
	@mkdir -p $(dir $@)
	$(if $(SILENT),,@echo [C++ ] $<)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Idaemon/include -Ilibs -Wno-old-style-cast -Wno-unused-parameter -MMD -MP -c -o $@ $<


out/storage/%.o: daemon/src/%.cpp
	@mkdir -p $(dir $@)
	$(if $(SILENT),,@echo [C++ ] $<)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Idaemon/include -Ilibs -MMD -MP -c -o $@ $<

out/storage/%.hpp.header: daemon/include/%.hpp
	@mkdir -p $(dir $@)
	$(if $(SILENT),,@echo [HDR ] $<)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Ilibs -c -fsyntax-only $<
	@touch $@

out/storage/%.o: storage/tests/%.cpp
	@mkdir -p $(dir $@)
	$(if $(SILENT),,@echo [C++ ] $<)
	$(hidecmd)$(CXX) $(CPPFLAGS) $(CXXFLAGS) -Idaemon/include -Ilibs -Wno-old-style-cast -Wno-unused-parameter -MMD -MP -c -o $@ $<
	

%/.dir:
	@mkdir -p $(@D) && touch $@

print:
	@echo "=== DAEMON DEBUG ==="
	@echo "sources: $(call sources,daemon)"
	@echo "objects: $(call objects,daemon)"
	@echo "header_checks: $(call header_checks,daemon)"
	@echo "out/daemon depends on: $(call objects,daemon) $(call header_checks,daemon)"


include $(wildcard $(patsubst %.o,%.d,$(objects) $(test_objects)))
