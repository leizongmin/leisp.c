CC=gcc
CFLAGS=-I.

DEPS=src/leisp.h

_OBJ=leisp.o cli.o
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
	$(CC) -o $(BDIR)/$@ $^ $(CFLAGS)

clean:
	rm -rf build
