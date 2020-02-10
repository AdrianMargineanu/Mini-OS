build:
	gcc -g *.c -Wall -Wextra -o tema2
run: build
	./tema2 test.txt test.out
rm :
	rm tema2
