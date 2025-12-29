CC=c99
CFLAGS=-Wall -g -O0

parsing: parsing.c mpc.c
	$(CC) $(CFLAGS) parsing.c mpc.c -ledit -lm -o parsing



