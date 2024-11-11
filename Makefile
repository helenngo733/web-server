main: main.c server.c
	gcc -o main main.c server.c

clean:
	rm main

test: main 
	bash test.sh