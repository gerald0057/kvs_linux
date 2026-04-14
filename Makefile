CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -I easyflash/inc -I inc
LDFLAGS =

EF_SRC = easyflash/src
EF_PORT = easyflash/port

EF_OBJS = \
	$(EF_SRC)/easyflash.o \
	$(EF_SRC)/ef_env.o \
	$(EF_SRC)/ef_env_legacy.o \
	$(EF_SRC)/ef_env_legacy_wl.o \
	$(EF_SRC)/ef_iap.o \
	$(EF_SRC)/ef_log.o \
	$(EF_PORT)/ef_utils.o \
	$(EF_PORT)/ef_port.o

APP = easyflash_test
APP_OBJ = easyflash_test.o
APP_DEPS = easyflash/inc/ef_linux.h

.PHONY: all clean

all: $(APP)

$(EF_SRC)/%.o: $(EF_SRC)/%.c $(wildcard $(EF_SRC)/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

$(EF_PORT)/%.o: $(EF_PORT)/%.c $(wildcard $(EF_PORT)/*.h)
	$(CC) $(CFLAGS) -c $< -o $@

$(APP): $(APP_OBJ) $(EF_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(APP_OBJ): $(APP_DEPS)

clean:
	rm -f $(EF_OBJS) $(APP_OBJ) $(APP)