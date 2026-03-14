# Makefile for stud_sys with ncurses support
# Usage: make             (bez MQTT)
#        make MQTT=1      (z MQTT, wymaga paho-mqtt-c)
#        make clean

CC     = gcc
CFLAGS = -D_NCURSES_ -I. -Iblank -Iopcjesys -Wall -Wno-format -Wno-pointer-sign -Wno-comment
LDFLAGS = -lncurses -lm
TARGET  = stud_sys
OBJDIR  = obj

# ----------------------------------------------------------------
# Opcjonalna obsluga MQTT (wymaga biblioteki paho-mqtt-c)
#   macOS:  brew install paho-mqtt-c
#   Linux:  sudo apt-get install libpaho-mqtt-dev
# ----------------------------------------------------------------
MQTT ?= 0
ifeq ($(MQTT),1)
    # Wykryj prefix: pkg-config → brew → /usr/local
    PAHO_PREFIX := $(shell pkg-config --variable=prefix paho-mqtt3c 2>/dev/null)
    ifeq ($(PAHO_PREFIX),)
        PAHO_PREFIX := $(shell brew --prefix paho-mqtt-c 2>/dev/null)
    endif
    ifeq ($(PAHO_PREFIX),)
        PAHO_PREFIX := /usr/local
    endif
    CFLAGS  += -D_MQTT_ -I$(PAHO_PREFIX)/include
    LDFLAGS += -L$(PAHO_PREFIX)/lib -lpaho-mqtt3c
    MQTT_SRC = mqtt_pub.c
else
    MQTT_SRC =
endif

# ----------------------------------------------------------------
# Pliki zrodlowe
# ----------------------------------------------------------------
SRCS = core.c \
       konfig.c \
       blank/term_ncurses.c \
       blank/konsola.c \
       blank/wewy.c \
       budz.c \
       admin.c \
       sys_rap.c \
       sprawdz.c \
       mqtt_pub.c \
       sensor_sim.c

OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(filter %.c,$(SRCS))) \
       $(patsubst %.C,$(OBJDIR)/%.o,$(filter %.C,$(SRCS)))

$(shell mkdir -p $(OBJDIR)/blank $(OBJDIR)/opcjesys)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)
	@echo "Build complete: $(TARGET)"
	@echo "Run with: ./$(TARGET)"

$(OBJDIR)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: %.C
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -x c -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(TARGET)
	@echo "Clean complete"

run: $(TARGET)
	./$(TARGET)

.SUFFIXES: .c .C .o
