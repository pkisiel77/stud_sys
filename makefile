# Makefile for stud_sys with ncurses support
# Usage: make             (bez MQTT)
#        make MQTT=1      (z MQTT, wymaga paho-mqtt-c)
#        make clean

CC     = gcc
CFLAGS = -D_NCURSES_ -I. -Iblank -Iopcjesys -Wall -Wno-format -Wno-pointer-sign -Wno-comment
LDFLAGS = -lncurses -lm
TARGET  = stud_sys
OBJDIR  = obj
DOC_VERSION := $(shell git rev-parse --short HEAD 2>/dev/null || echo unknown)
DOC_DATE := $(shell date +%F)

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

.PHONY: all clean docs-pdf

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

docs-pdf:
	@mkdir -p docs/pdf docs/assets
	cd docs/assets && xelatex -interaction=nonstopmode -halt-on-error stud_sys_logo.tex >/dev/null
	@rm -f docs/assets/stud_sys_logo.aux docs/assets/stud_sys_logo.log
	sed -e 's/@DOC_VERSION@/$(DOC_VERSION)/g' -e 's/@DOC_DATE@/$(DOC_DATE)/g' docs/pdf_header.tex.in > docs/.pdf_header.generated.tex
	sed -e 's/@DOC_VERSION@/$(DOC_VERSION)/g' -e 's/@DOC_DATE@/$(DOC_DATE)/g' docs/title_page.md.in > docs/.title_page.generated.md
	sed -e 's/@DOC_VERSION@/$(DOC_VERSION)/g' -e 's/@DOC_DATE@/$(DOC_DATE)/g' docs/presentation_title_page.md.in > docs/.presentation_title_page.generated.md
	pandoc docs/user_guide.md -o docs/pdf/user_guide.pdf --pdf-engine=xelatex -H docs/.pdf_header.generated.tex
	pandoc docs/presentation_budzik.md -o docs/pdf/presentation_budzik.pdf --pdf-engine=xelatex -H docs/.pdf_header.generated.tex
	pandoc docs/operator_cheatsheet.md -o docs/pdf/operator_cheatsheet.pdf --pdf-engine=xelatex -H docs/.pdf_header.generated.tex
	pandoc docs/.title_page.generated.md docs/quick_start.md docs/user_guide.md docs/operator_cheatsheet.md docs/presentation_budzik.md -o docs/pdf/stud_sys_docs_bundle.pdf --pdf-engine=xelatex -H docs/.pdf_header.generated.tex
	pandoc docs/.presentation_title_page.generated.md docs/operator_cheatsheet.md docs/presentation_budzik.md -o docs/pdf/stud_sys_presentation_pack.pdf --pdf-engine=xelatex -H docs/.pdf_header.generated.tex
	@echo "PDF export complete: docs/pdf/"

.SUFFIXES: .c .C .o
