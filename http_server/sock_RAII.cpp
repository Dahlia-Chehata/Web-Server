#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "sock_RAII.h"

sock_RAII::sock_RAII(int sock_fd) {
	socket_fd = sock_fd;
}
sock_RAII::~sock_RAII() {
	shutdown(socket_fd, SHUT_RDWR);
	close(socket_fd);
}
