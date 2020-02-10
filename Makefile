build:
	gcc -g *.c -Wall -Wextra -o tema2
run: build
	./minios input.txt output.out
rm :
	rm minios
