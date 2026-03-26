# Makefile – Projeto 1: Processamento de Imagens
# Disciplina: Computação Visual – Mackenzie
# Compilador: gcc (C99)

CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -pedantic -O2 \
           $(shell pkg-config --cflags sdl3 SDL3_image SDL3_ttf 2>/dev/null || \
             echo "-I/usr/local/include/SDL3")
LDFLAGS = $(shell pkg-config --libs sdl3 SDL3_image SDL3_ttf 2>/dev/null || \
            echo "-lSDL3 -lSDL3_image -lSDL3_ttf") -lm

TARGET  = programa
SRCDIR  = src
OBJDIR  = build

SRCS    = $(wildcard $(SRCDIR)/*.c)
OBJS    = $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(SRCS))

.PHONY: all clean

all: $(OBJDIR) $(TARGET)

$(OBJDIR):
	mkdir -p $(OBJDIR)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf $(OBJDIR) $(TARGET) output_image.png
