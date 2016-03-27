SOURCES = snowflake.c algo.c imageOut.c bsp.c
HEADERS = algo.h imageOut.h bsp.h

OUTPUT = snowflake

CFLAGS = -std=c99 -Wall
LIBS = -lm

all: $(OUTPUT)

$(OUTPUT): $(SOURCES) $(HEADERS)
	gcc $(SOURCES) $(CFLAGS) -o $@ $(LIBS)

test: bsp_test bsp_test.c bsp.c bsp.h
	./bsp_test

bsp_test: bsp_test.c bsp.c bsp.h algo.c algo.h
	gcc bsp_test.c bsp.c algo.c $(CFLAGS) -o $@ $(LIBS)

clean:
	rm -f $(OUTPUT) bsp_test