




LIST=bayes bdump

all: $(LIST)

install: 
	cp $(LIST) *.exe ../bin



CFLAGS = -g
#CFLAGS = -O5
LDFLAGS = -lm

correlat_bihourly: correlat_bihourly.c gen.o hash.o


bayes: bayes.c gen.o hash.o

bdump: bdump.c gen.o hash.o

simplify: simplify.c gen.o 


make_money: make_money.c gen.o hash.o


nnl: nnl.c gen.o liblwneuralnet.a hash.o

creator: creator.o gen.o liblwneuralnet.a 

ticker_from_archive: ticker_from_archive.c gen.o hash.o ai_constant.h
normalized_from_archive: normalized_from_archive.c gen.o hash.o ai_constant.h

ai: ai.c gen.o liblwneuralnet.a ai_constant.h

nntest: nntest.c gen.o liblwneuralnet.a hash.o

adamize: adamize.c

gen.o: gen.c


hash.o: hash.c hash.h
