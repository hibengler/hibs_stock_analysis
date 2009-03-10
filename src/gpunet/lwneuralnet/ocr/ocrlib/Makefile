# Makefile -- makefile for ocr application
# $Id$

CC := gcc
CFLAGS += -O2 -g -Wall -W -I../../source

libocr.a : train.o sample.o fileio.o
	ar cru $@ $^
	ranlib $@
 
indent :
	indent -TFILE -TgzFile -Ttraining_data_t -Tnetwork_t --no-tabs *.[ch]

clean : 
	-rm -f *.o libocr.a

clean-backup :
	-rm -f *.bak *~

clean-all : clean clean-backup

source-dist :
	-rm -f *.o libocr.a
	-rm -f *.bak *~
	-rm -f .cvsignore
