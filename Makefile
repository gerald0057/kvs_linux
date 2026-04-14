CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -Wno-unused-parameter -Wno-type-limits
LDFLAGS =

# Source and include directories
SRC_DIRS   = easyflash/src easyflash/port crc
INC_DIRS   = easyflash/inc inc crc
BUILD_DIR  = build
CONFIG_DIR = config

# Kconfig
KCONFIG_CONFIG    = $(CONFIG_DIR)/.config
KCONFIG_AUTOCONF  = inc/kvs_config.h

# Auto-discover source files
SRCS_C  := $(shell find $(SRC_DIRS) -name '*.c' 2>/dev/null)
OBJS    := $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRCS_C))
APP_OBJ := $(BUILD_DIR)/kvs_linux.o

# Build flags
INC_FLAGS := $(addprefix -I ,$(INC_DIRS))
DEP_FLAGS  = -MMD -MP -MF $(@:.o=.d)

# External CRC32 flag (from kconfig)
ifneq ($(wildcard $(KCONFIG_CONFIG)),)
include $(KCONFIG_CONFIG)
ifeq ($(CONFIG_USING_EXTERNAL_CRC32),y)
CFLAGS += -DCONFIG_USING_EXTERNAL_CRC32
endif
endif

TARGET = kvs_linux

.PHONY: all clean config menuconfig defconfig savedefconfig

all: config_check $(BUILD_DIR) $(KCONFIG_AUTOCONF) $(TARGET)

# --- Kconfig rules ---

config_check:
	@if [ ! -f $(KCONFIG_CONFIG) ]; then \
		echo "No configuration found. Running 'make defconfig'..."; \
		$(MAKE) --no-print-directory defconfig; \
	fi

defconfig:
	@mkdir -p $(CONFIG_DIR)
	python3 -c "from kconfiglib import Kconfig; kconf = Kconfig('Kconfig'); kconf.write_config('$(KCONFIG_CONFIG)')"
	KCONFIG_CONFIG=$(KCONFIG_CONFIG) genconfig --header-path $(KCONFIG_AUTOCONF)

menuconfig:
	@mkdir -p $(CONFIG_DIR)
	KCONFIG_CONFIG=$(KCONFIG_CONFIG) menuconfig Kconfig
	KCONFIG_CONFIG=$(KCONFIG_CONFIG) genconfig --header-path $(KCONFIG_AUTOCONF)

savedefconfig:
	KCONFIG_CONFIG=$(KCONFIG_CONFIG) savedefconfig --kconfig Kconfig --out $(CONFIG_DIR)/defconfig

$(KCONFIG_AUTOCONF): $(KCONFIG_CONFIG) Kconfig
	KCONFIG_CONFIG=$(KCONFIG_CONFIG) genconfig --header-path $(KCONFIG_AUTOCONF)

config:
	@cat $(KCONFIG_CONFIG) 2>/dev/null || echo "No .config found. Run 'make menuconfig'."

# --- Build rules ---

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)
	@for d in $(SRC_DIRS); do mkdir -p $(BUILD_DIR)/$$d; done

$(BUILD_DIR)/%.o: %.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC_FLAGS) $(DEP_FLAGS) -c $< -o $@

$(TARGET): $(APP_OBJ) $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC_FLAGS) $^ -o $@ $(LDFLAGS)

$(APP_OBJ): kvs_linux.c $(KCONFIG_AUTOCONF) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(INC_FLAGS) $(DEP_FLAGS) -c $< -o $@

# Include auto-generated dependencies
-include $(OBJS:%.o=%.d) $(APP_OBJ:%.o=%.d)

clean:
	@rm -rf $(BUILD_DIR) cache
	@find . -name "*.o" -delete 2>/dev/null || true