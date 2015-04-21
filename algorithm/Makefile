CC=gcc
CFLAGS=-g
DIRS=$(notdir $(shell find ./* -type d))
EXES=$(addsuffix .exe,$(DIRS))

all:$(EXES)

%.exe:
	@echo "Make $@"
	${CC} ${CFLAGS} sort_framework.c $(basename $@)/*.c -o $@

.PHONY: clean

clean:
	rm $(EXES) -f
