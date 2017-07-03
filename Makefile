CFLAGS=-Wall -pedantic `glib-config --cflags`
DEBUG=-ggdb -g3
BEASTOBJS=main.o beast.o console.o sig.o
CC=gcc
LDADD=-lglib -lncurses

beasts: ${BEASTOBJS}
	${CC} -o $@ ${BEASTOBJS} ${DEBUG} ${LDADD}

.c.o:
	${CC} ${CFLAGS} ${DEBUG} -c $<
