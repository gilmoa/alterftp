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

test : alterftp
	./$^
