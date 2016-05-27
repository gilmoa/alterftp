CC = gcc
CFLAGS = -Wall

alterftp : alterftp.c
	$(CC) $^ -o $@ $(CFLAGS)

install : alterftp
	cp $^ /usr/bin/$^
	chmod a+x /usr/bin/$^

.PHONY : clean

clean :
	@rm -f *~ alterftp

test : alterftp
	./$^
