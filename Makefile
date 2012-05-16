NAME = xyz123
FILES = main args xyz buffs ezflate bmp macros list

CC ?= gcc
CFLAGS += -ansi -pedantic -Wall -Werror
LDFLAGS = -lz

PREFIX ?= /usr/local
PROGRAM = bin/$(NAME)
SRCS = $(FILES:%=src/%.c)
OBJS = $(FILES:%=bin/%.o)

DEBUG_FOOTPRINT = bin/_debug
RELEASE_FOOTPRINT = bin/_release

.PHONY: all
all: release

.PHONY: release
release: CFLAGS += -O3
release: $(RELEASE_FOOTPRINT) $(PROGRAM)

.PHONY: debug
debug: CFLAGS += -O0
debug: LDFLAGS += -g
debug: $(DEBUG_FOOTPRINT) $(PROGRAM)

$(RELEASE_FOOTPRINT):
	make clean
	touch $(RELEASE_FOOTPRINT)

$(DEBUG_FOOTPRINT):
	make clean
	touch $(DEBUG_FOOTPRINT)

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $(LDFLAGS) $(OBJS)

bin/%.o: src/%.c src/%.h
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) -c $<

.PHONY: clean
clean:
	-rm -f $(PROGRAM) $(OBJS) $(RELEASE_FOOTPRINT) $(DEBUG_FOOTPRINT)

.PHONY: install
install: all
	cp $(PROGRAM) $(PREFIX)/bin

