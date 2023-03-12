all: build

build:
	gcc -std=c99 -g *.c -o sd_fs

clean:
	rm *.o sd_fs

run:
	./sd_fs