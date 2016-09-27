CC=gcc
CFLAGS=-std=c11 -pedantic-errors -Wall -Wextra -Werror -O2
ODIR=obj

_OBJ = quoted.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: %.c
	@mkdir -p $(@D)
	$(CC) -c -o $@ $< $(CFLAGS)

quoted: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o quoted
