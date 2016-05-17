CC=gcc
CFLAGS=-I. -std=c99 -Wall
CFLAGS_LINK=-ledit -lm
CFLAGS_DEBUG=-g

DEPS=

_OBJ=parsing.o mpc.o repl.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

ODIR=build/obj
BDIR=out


all:
	make clean
	mkdir -p $(ODIR)
	mkdir -p $(BDIR)
	make repl
	make repl_debug

$(ODIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

repl: $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(CFLAGS_LINK)

repl_debug: $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(CFLAGS_LINK) $(CFLAGS_DEBUG)

clean:
	rm -rf build
	rm -rf out
