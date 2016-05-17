CC=gcc
CFLAGS=-I. -std=c99 -Wall
CFLAGS2=-ledit -lm

DEPS=

_OBJ=parsing.o mpc.o
OBJ=$(patsubst %,$(ODIR)/%,$(_OBJ))

ODIR=build/obj
BDIR=out


all:
	make clean
	mkdir -p $(ODIR)
	mkdir -p $(BDIR)
	make cli

$(ODIR)/%.o: src/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

cli: $(OBJ)
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS) $(CFLAGS2)

clean:
	rm -rf build
