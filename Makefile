NAME = xyz123
FILES = main args xyz buffs easyflate bmp macros list

CC ?= gcc
CFLAGS += -ansi -pedantic -Wall -Werror
LDFLAGS = -lz

PREFIX ?= /usr/local
PROGRAM = bin/$(NAME)
SRCS = $(FILES:%=src/%.c)
OBJS = $(FILES:%=bin/%.o)

.PHONY: all
all: $(PROGRAM)

.PHONY: debug
debug: CFLAGS += -O0
debug: LDFLAGS += -g
debug: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS)

bin/%.o: src/%.c src/%.h
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) -c $<

.PHONY: clean
clean:
	-rm -f $(PROGRAM) $(OBJS)

.PHONY: install
install: all
	cp $(PROGRAM) $(PREFIX)/bin

