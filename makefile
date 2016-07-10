CFLAGS = -std=c99 -Wall -O3
LIBS = -lm

OBJS = src/algo.o src/arg_parsing.o src/bsp.o src/bsp_test.o src/render_bsp.o src/snowflake.o src/snowflake_gen.o src/tga.o

all: snowflake

snowflake: $(OBJS)
	gcc $^ $(CFLAGS) -o $@ $(LIBS)

%.o: %.c
	gcc -o $@ -c $<

src/algo.o: src/algo.h
src/arg_parsing.o: src/algo.h src/arg_parsing.h
src/bsp.o: src/algo.h src/bsp.h
src/bsp_test.o: src/bsp.h src/bsp_test.h
src/render_bsp.o: src/algo.h src/bsp.h src/tga.h src/render_bsp.h
src/snowflake.o: src/arg_parsing.h src/bsp.h src/bsp_test.h src/render_bsp.h src/snowflake_gen.h
src/snowflake_gen.o: src/algo.h src/bsp.h src/snowflake_gen.h
src/tga.o: src/tga.h

test: snowflake
	./snowflake bsp_test

clean:
	rm -rf $(OBJS) snowflake
