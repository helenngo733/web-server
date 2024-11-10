main: main.c http_msg.c
	gcc -o main main.c http_msg.c

clean:
	rm main