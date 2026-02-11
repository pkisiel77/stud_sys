# Makefile for stud_sys with ncurses support
# Usage: make
# To compile: make
# To clean: make clean

CC = gcc
CFLAGS = -D_NCURSES_ -I. -Iblank -Iopcjesys -Wall -Wno-format -Wno-pointer-sign
LDFLAGS = -lncurses -lm
TARGET = stud_sys
OBJDIR = obj

# Source files
SRCS = core.c \
       blank/term_ncurses.c \
       blank/konsola.c \
       blank/wewy.c \
       Budz.c \
       ADMIN.C \
       POMIAR.C \
       SYS_RAP.C \
       SPRAWDZ.C \
       URUCHOM.C

# Object files
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(filter %.c,$(SRCS))) \
       $(patsubst %.C,$(OBJDIR)/%.o,$(filter %.C,$(SRCS)))

# Create object directories
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
