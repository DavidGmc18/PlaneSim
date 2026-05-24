include config.mk

.PHONY: all always clean build

all: always build

always:
	@mkdir -p $(BUILD_DIR)


clean:
	@test -n "$(BUILD_DIR)" && test "$(BUILD_DIR)" != "/" && rm -rf $(BUILD_DIR)/*


SOURCES := $(shell find $(SRC_DIR) -type f \( -name "*.cpp" -o -name "*.c" \))
OBJECTS := $(patsubst $(SRC_DIR)/%, $(BUILD_DIR)/obj/%.o, $(SOURCES))
DEPS := $(OBJECTS:.o=.d)

build: $(BINARY)

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