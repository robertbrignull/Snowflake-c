CFLAGS = -std=c99 -Wall
LIBS = -lm

SOURCES = src/algo.c src/arg_parsing.c src/bsp.c src/bsp_test.c src/image_out.c src/snowflake.c src/snowflake_gen.c
HEADCERS = src/algo.h src/arg_parsing.h src/bsp.h src/bsp_test.h src/image_out.h src/snowflake_gen.h

all: snowflake

snowflake: $(SOURCES) $(HEADCERS)
	gcc $(SOURCES) $(CFLAGS) -o $@ $(LIBS)

test: snowflake
	./snowflake bsp_test

clean:
	rm -f snowflake
