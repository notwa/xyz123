NAME = xyz123
FILES = main args xyz buffs gif macros node

CC ?= gcc
CFLAGS += -pedantic -Wall -Werror
LDFLAGS += -lz -lgif

PREFIX ?= /usr/local
PROGRAM = bin/$(NAME)
SRCS = $(FILES:%=src/%.c)
OBJS = $(FILES:%=bin/%.o)

DEBUG_FOOTPRINT = bin/_debug
RELEASE_FOOTPRINT = bin/_release

.PHONY: all debug clean install

all: $(RELEASE_FOOTPRINT) $(PROGRAM)

debug: CFLAGS += -O0 -g
debug: LDFLAGS += -g
debug: $(DEBUG_FOOTPRINT) $(PROGRAM)

$(RELEASE_FOOTPRINT):
	make clean
	touch $(RELEASE_FOOTPRINT)

$(DEBUG_FOOTPRINT):
	make clean
	touch $(DEBUG_FOOTPRINT)

$(PROGRAM): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LDFLAGS) 

bin/%.o: src/%.c src/%.h
	$(CC) -o $@ $(CFLAGS) -c $<

clean:
	-rm -f $(PROGRAM) $(OBJS) $(RELEASE_FOOTPRINT) $(DEBUG_FOOTPRINT)

install: all
	cp $(PROGRAM) $(PREFIX)/bin

