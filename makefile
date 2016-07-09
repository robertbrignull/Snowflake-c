CFLAGS = -std=c99 -Wall
LIBS = -lm

all: snowflake

snowflake: src/algo.c src/bsp.c src/image_out.c src/snowflake.c src/snowflake_gen.c src/algo.h src/bsp.h src/image_out.h src/snowflake_gen.h
	gcc src/algo.c src/bsp.c src/image_out.c src/snowflake.c src/snowflake_gen.c $(CFLAGS) -o $@ $(LIBS)

test: bsp_test
	./bsp_test

bsp_test: src/algo.c src/bsp.c src/bsp_test.c src/algo.h src/bsp.h
	gcc src/bsp_test.c src/bsp.c src/algo.c $(CFLAGS) -o $@ $(LIBS)

clean:
	rm -f snowflake bsp_test
