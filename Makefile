include config.mk

.PHONY: all always clean build run run-hud

all: always build

always:
	@mkdir -p $(BUILD_DIR)
	@if [ "$(BUILD_ON_RAM)" = "1" ]; then \
		mountpoint -q $(BUILD_DIR) || sudo mount -t tmpfs -o size=$(BUILD_ON_RAM_SIZE) tmpfs $(BUILD_DIR); \
	else \
		mountpoint -q $(BUILD_DIR) && sudo umount $(BUILD_DIR) || true; \
	fi

clean:
	@test -n "$(BUILD_DIR)" && test "$(BUILD_DIR)" != "/" && rm -rf $(BUILD_DIR)/*


SOURCES := $(shell find $(SRC_DIR) -type f \( -name "*.cpp" -o -name "*.c" \))
OBJECTS := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/obj/src/%.o, $(SOURCES))
DEPS := $(OBJECTS:.o=.d)

EXTERNAL_SOURCES := $(shell find $(EXTERNAL_DIR) -type f \( -name "*.cpp" -o -name "*.c" \))
EXTERNAL_OBJECTS := $(patsubst $(EXTERNAL_DIR)/%, $(BUILD_DIR)/obj/external/%.o, $(EXTERNAL_SOURCES))

build:
	@$(MAKE) $(BINARY) -j$(shell nproc) --output-sync=target --no-print-directory
	@echo "Done!"

$(BINARY): $(OBJECTS) $(EXTERNAL_OBJECTS)
	@mkdir -p $(@D)
	@echo "Linking $(subst $(SOURCE_DIR)/,,$@)"
	@$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)


# OBJECTS
$(BUILD_DIR)/obj/src/%.o: $(SRC_DIR)/%
	@mkdir -p $(@D)
	@echo "Compiling $(subst $(SOURCE_DIR)/,,$<)"
	@$(CXX) $(CXXFLAGS_SRC) $(CXXFLAGSINTERNAL) $(INCLUDES) -MMD -MP -c $< -o $@


# EXTERNAL OBJECTS
$(BUILD_DIR)/obj/external/%.o: $(EXTERNAL_DIR)/%
	@mkdir -p $(@D)
	@echo "Compiling $(subst $(SOURCE_DIR)/,,$<)"
	@$(CXX) $(CXXFLAGS_EXTERNAL) $(INCLUDES) -MMD -MP -c $< -o $@


# DEPS
-include $(DEPS)


# RUN
run:
	@$(BINARY)

run-hud:
	@mangohud --dlsym $(BINARY)