

LIST=simplify correlat creator correlat_bihourly make_money nnl nntest adamize \
ticker_from_archive normalized_from_archive ai \
step1 step2 step3_launch step3 creator2 archive_to_ticker normalize train try \
compute_error_rate

all: $(LIST)

install: 
	cp $(LIST) *.exe ../bin

#CFLAGS = -O3 -march=pentium4m 
#CFLAGS = -O3 

#CFLAGS = -O3 -march=prescott
CFLAGS = -g
LDFLAGS = -lm

correlat_bihourly: correlat_bihourly.c gen.o hash.o


correlat: correlat.c gen.o hash.o

simplify: simplify.c gen.o 


make_money: make_money.c gen.o hash.o


nnl: nnl.c gen.o liblwneuralnet.a hash.o

creator: creator.o gen.o liblwneuralnet.a 

creator2: creator2.o gen.o liblwneuralnet.a 

archive_to_ticker: archive_to_ticker.c gen.o hash.o ai_constant.h

ticker_from_archive: ticker_from_archive.c gen.o hash.o ai_constant.h
normalized_from_archive: normalized_from_archive.c gen.o hash.o ai_constant.h

ai: ai.c gen.o liblwneuralnet.a ai_constant.h

train: train.c gen.o liblwneuralnet.a ai_constant.h

compute_error_rate: compute_error_rate.c gen.o liblwneuralnet.a ai_constant.h

try: try.c gen.o liblwneuralnet.a ai_constant.h

nntest: nntest.c gen.o liblwneuralnet.a hash.o

adamize: adamize.c

gen.o: gen.c

normalize: normalize.c gen.c

hash.o: hash.c hash.h
