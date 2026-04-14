CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I easyflash/inc -I inc -I crc
LDFLAGS =

EF_SRC = easyflash/src
EF_PORT = easyflash/port
CRC_DIR = crc

EF_OBJS = \
	$(EF_SRC)/easyflash.o \
	$(EF_SRC)/ef_env.o \
	$(EF_SRC)/ef_env_legacy.o \
	$(EF_SRC)/ef_env_legacy_wl.o \
	$(EF_SRC)/ef_iap.o \
	$(EF_SRC)/ef_log.o \
	$(EF_SRC)/ef_utils.o \
	$(EF_PORT)/ef_port.o

CRC_OBJ = $(CRC_DIR)/crc32.o

APP = kvs_linux
APP_OBJ = easyflash_test.o
APP_DEPS = easyflash/inc/ef_linux.h

.PHONY: all clean

all: $(APP)

$(CRC_DIR)/crc32.o: $(CRC_DIR)/crc32.c $(CRC_DIR)/crc.h
	$(CC) $(CFLAGS) -DCONFIG_USING_EXTERNAL_CRC32 -c $< -o $@

$(EF_SRC)/%.o: $(EF_SRC)/%.c
	$(CC) $(CFLAGS) -DCONFIG_USING_EXTERNAL_CRC32 -c $< -o $@

$(EF_PORT)/%.o: $(EF_PORT)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(APP): $(APP_OBJ) $(EF_OBJS) $(CRC_OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(APP_OBJ): $(APP_DEPS)

clean:
	rm -f $(EF_OBJS) $(CRC_OBJ) $(APP_OBJ) $(APP)