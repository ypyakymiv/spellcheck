server: server.h server.c request.h request.c dict.h dict.c log.h log.c utils.h utils.c
	gcc -g server.c request.c dict.c log.c utils.c -o server -pthread
