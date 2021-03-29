server: server.h server.c request.h request.c dict.h dict.c utils.h utils.c
	gcc -g server.c request.c dict.c utils.c -o server -pthread
