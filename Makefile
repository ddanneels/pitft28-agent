# Thanks to https://makefiletutorial.com/
TARGET_BIN := agent

BUILD_DIR := ./build

LDFLAGS ?= -lm -lts -pthread

# A very long list of compilation flags
CFLAGS ?= -O3 -g0 -std=c11 -Wall -Wshadow -Wundef \
	-Wall -Wextra -Wno-unused-function -Wno-error=strict-prototypes \
	-Wpointer-arith -fno-strict-aliasing -Wuninitialized -Wdeprecated \
	-Wno-unused-parameter -Wno-missing-field-initializers -Wtype-limits \
	-Wno-format-nonliteral -Wno-cast-qual -Wunreachable-code -Wempty-body \
	-Wno-switch-default -Wreturn-type -Wmultichar -Wformat-security \
	-Wno-ignored-qualifiers -Wno-sign-compare \
	-Wclobbered -Wno-unused-value

SRC_DIRS := ./src ./lvgl ./lv_drivers ./assets

SRCS := $(shell find $(SRC_DIRS) -name '*.c')

# CC ?= gcc
CC ?= gcc

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)

# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := -I. $(addprefix -I,$(INC_DIRS))

# All objects will be built inside BUILD_DIR
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# Ask gcc to generate dependencies makefiles
CFLAGS += -MMD -MP

# Those dependencies will have a .d extension
DEPS := $(OBJS:.o=.d)


# First and default target
all: $(BUILD_DIR)/$(TARGET_BIN)

# Final linking
$(BUILD_DIR)/$(TARGET_BIN): $(OBJS)
	@echo "  Building $@"
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	@echo "  CC $<"
	@$(CC) $(CFLAGS) $(INC_FLAGS) -c $< -o $@


.PHONY: clean deploy
clean:
	rm -rf $(BUILD_DIR)


# deploy: all
# 	rcp $(BUILD_DIR)/$(TARGET_BIN) root@tabbee:/home/root/homescreen.tmp
# 	echo "killall homescreen ; cp -p homescreen.tmp homescreen" | ssh root@tabbee '/bin/sh -s' 

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)