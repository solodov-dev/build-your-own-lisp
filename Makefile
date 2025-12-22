CC=c99
CFLAGS=-Wall

parsing: parsing.c mpc.c
	$(CC) $(CFLAGS) parsing.c mpc.c -ledit -lm -o parsing



