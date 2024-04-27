
override CFLAGS += -Wall -O3

sources := $(wildcard src/*.c)
objects := $(sources:src/%.c=obj/%.o)

all: output_dir sk_ungroup sk_gfx2bmp sk_sfx2wav

sk_ungroup:  obj/cmd_param.o obj/cache.o obj/sk_ungroup.o
	${CC} -o $@    $^ $(LDFLAGS)

sk_gfx2bmp:  obj/cmd_param.o obj/cache.o obj/sk_gfx2bmp.o obj/bmp_file.o
	${CC} -o $@    $^ $(LDFLAGS)

sk_sfx2wav:  obj/cmd_param.o obj/cache.o obj/sk_sfx2wav.o obj/sfxunpacker.o obj/wave.o
	${CC} -o $@    $^ $(LDFLAGS)

$(objects): obj/%.o: src/%.c
	${CC} -o $@ $^ -c $(CFLAGS)

output_dir:
	@mkdir -p obj

clean:
	rm -Rf  *.o  .*.o  .*.o.* sk_ungroup sk_gfx2bmp sk_sfx2wav obj

