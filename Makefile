all: Parcial2 clear start remove

Parcial2: main.c
		gcc main.c -g -o Parcial2

start: Parcial2
	./Parcial2

clear:
	clear

remove:
	rm Parcial2