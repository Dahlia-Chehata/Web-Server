#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "sock_RAII.h"

sock_RAII::sock_RAII(int sock_fd) {
	socket_fd = sock_fd;
}
sock_RAII::~sock_RAII() {
	close(socket_fd);
}
