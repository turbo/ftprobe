#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <arpa/ftp.h>
#include <netdb.h>
#include <stdarg.h>
#include <unistd.h>
#include <signal.h>
#include <getopt.h>
#include <ctype.h>
#include <sys/stat.h>

#define MAX_STR	2048

char ip[MAX_STR];
enum {FALSE, TRUE};
int fd;

FILE * tcp_connect(char *server, char *port, char *mode) {
	struct addrinfo ai, *srv = NULL, *p = NULL;
	struct timeval timeout;

	timeout.tv_sec = 1;
	timeout.tv_usec = 0;

	memset(&ai, 0, sizeof(struct addrinfo));
	if (( getaddrinfo(server, port, &ai, &srv)) != 0 ) exit(0);

	ai.ai_family = AF_UNSPEC;
	ai.ai_socktype = SOCK_STREAM;
	ai.ai_protocol = IPPROTO_TCP;

	p = srv;
	if (( getnameinfo((struct sockaddr *)p->ai_addr,
					p->ai_addrlen,
					ip, sizeof(ip),
					NULL, (socklen_t) 0U,
					NI_NUMERICHOST)) != 0)
		exit(0);

	if (( fd = socket(p->ai_family, p->ai_socktype, 0)) < 0 ) exit(0);

	if ( connect(fd, p->ai_addr, p->ai_addrlen) < 0 ) exit(0);
	freeaddrinfo(srv);

	if (setsockopt (fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout,
				sizeof(timeout)) < 0) exit(0);
	if (setsockopt (fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout,
				sizeof(timeout)) < 0) exit(0);

	return (fdopen(fd, mode));
}

int sendcmd(FILE *stream, const char * cmd) {
	char line[MAX_STR];
	fprintf(stream, "%s\r\n", cmd);
	while (( fgets(line, sizeof(line), stream)) != NULL );
	return atoi(line);
}

void signal_handler(int sig) {
	if (!(sig == SIGINT)) return;
	exit(close(fd));
}

int main(int argc, char **argv) {
	FILE * stream;
	signal(SIGINT, signal_handler);
	int max_tries = 8;

	stream = tcp_connect(argv[1], "21", "r+");
	if (!stream) exit(0);

	argc = sendcmd(stream, "USER anonymous");

	if (argc == 331 && argc != 230)
		while (!(argc == 230 || argc == 503) && max_tries--)
			argc = sendcmd(stream, "PASS anonymous@");

	if (max_tries) puts("OK");

	fprintf(stream, "QUIT\r\n");
	fclose(stream);

	return 0;
}
