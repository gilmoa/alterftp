CC = gcc
CFLAGS = -Wall

alterftp : alterftp.c debug.c
	$(CC) $< -o $@ $(CFLAGS)

install : alterftp
	cp $^ /usr/bin/$^
	chmod a+x /usr/bin/$^

.PHONY : clean

clean :
	@rm -f *~ alterftp

test1 : alterftp
	./$^ help

test2 : alterftp
	./$^ init file1 file2

test3 : alterftp
	./$^ send

test4 : alterftp
	./$^ send file1 file2 file3

test : test1 test2 test3 test4
