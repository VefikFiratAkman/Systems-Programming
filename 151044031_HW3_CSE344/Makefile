vfa: compile prog1 prog2 prog3 prog4 prog5

compile:
	gcc -c gtushell.c -ansi -Wall -pedantic-errors -std=gnu99
	gcc -o gtushell gtushell.o

prog1:
	gcc -o cat cat.c

prog2:
	gcc -o lsf lsf.c

prog3:
	gcc -o pwd pwd.c

prog4:
	gcc -o wc wc.c
	
prog5:
	gcc -o bunedu bunedu.c


run:
	./shell A

clean:
	rm -f *.o gtushell cat lsf pwd wc bunedu
	clear

