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
OBJECTS := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/obj/%.o, $(SOURCES))
DEPS := $(OBJECTS:.o=.d)

build:
	@$(MAKE) $(BINARY) -j$(shell nproc) --output-sync=target --no-print-directory

$(BINARY): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)


# OBJECTS
$(BUILD_DIR)/obj/%.o: $(SRC_DIR)/%
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@


# DEPS
-include $(DEPS)


# RUN
run:
	@$(BINARY)

run-hud:
	@mangohud --dlsym $(BINARY)