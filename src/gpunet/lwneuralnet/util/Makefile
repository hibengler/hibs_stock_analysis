CC := gcc
CFLAGS += -g -O2 -Wall -W -I../source
LIBLW = ../source/liblwneuralnet.a

all : lwcreate lwrandomize lwcopy

lwcreate : lwcreate.o
	$(CC) -o $@ $^ $(LIBLW) -lm

lwrandomize : lwrandomize.o
	$(CC) -o $@ $^ $(LIBLW) -lm

lwcopy : lwcopy.o
	$(CC) -o $@ $^ $(LIBLW) -lm

indent :
	indent -Tneuron_t -Tlayer_t -Tnetwork_t -TFILE --no-tabs -kr -i2 -cli2 *.c

clean :
	-rm -f *.o
