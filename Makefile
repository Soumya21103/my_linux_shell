all:
	gcc myshell.c -o myshell -pthread
	gcc ls.c -o ls
	gcc mkdir.c -o mkdir
	gcc cat.c -o cat
	gcc date.c -o date
	gcc rm.c -o rm