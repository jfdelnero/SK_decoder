
override CFLAGS += -Wall -O3

sources := $(wildcard src/*.c)
objects := $(sources:src/%.c=obj/%.o)

all: output_dir sk_ungroup sk_gfx2bmp sk_sfx2wav sk_maz

sk_ungroup:  obj/cmd_param.o obj/cache.o obj/sk_ungroup.o obj/utils.o
	${CC} -o $@    $^ $(LDFLAGS)

sk_gfx2bmp:  obj/cmd_param.o obj/cache.o obj/sk_gfx2bmp.o obj/bmp_file.o obj/utils.o obj/sk_gfx.o
	${CC} -o $@    $^ $(LDFLAGS)

sk_sfx2wav:  obj/cmd_param.o obj/cache.o obj/sk_sfx2wav.o obj/sfxunpacker.o obj/wave.o obj/utils.o
	${CC} -o $@    $^ $(LDFLAGS)

sk_maz:  obj/cmd_param.o obj/cache.o obj/sk_maz.o obj/bmp_file.o obj/utils.o obj/font4x6.o obj/sk_id.o obj/sk_gfx.o
	${CC} -o $@    $^ $(LDFLAGS)

$(objects): obj/%.o: src/%.c
	${CC} -o $@ $^ -c $(CFLAGS)

output_dir:
	@mkdir -p obj

clean:
	rm -Rf  *.o  .*.o  .*.o.* sk_ungroup sk_gfx2bmp sk_sfx2wav sk_maz obj

