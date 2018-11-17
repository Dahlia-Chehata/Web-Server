#ifndef SOCKET_RAII_H_
#define SOCKET_RAII_H_

 /*
 * Resource acquisition is initialization.
 * this is used to make sure the socket file descriptor 
 * is always closed when the object it contains goes out of scope.
 */
class sock_RAII{
	private:
		int socket_fd;
	public:
		sock_RAII(int sock_fd);
		~sock_RAII();
};

#endif //SOCKET_RAII_H_
