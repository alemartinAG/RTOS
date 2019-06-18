CC=gcc
CFLAGS=-Werror -Wall -pedantic

all:
	$(CC) $(CFLAGS) prod_temp.c -o prod_temp.out -lrt
clean:
	rm *.out
cppcheck:
	cppcheck --enable=all  --suppress=missingIncludeSystem *.c 2>cpp_check.txt
