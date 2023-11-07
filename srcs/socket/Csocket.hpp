#ifndef CSOCKET_HPP
#define CSOCKET_HPP

#include <sys/socket.h>
#include <netinet/in.h>
void fatal(const char *msg);


class Csocket
{	
public:
	u_int32_t	getfd(void);
	sockaddr_in getAddr(void);
protected:
	struct sockaddr_in addr;
	socklen_t addr_size;
	int32_t fd_socket;
	Csocket();
	~Csocket();
};

#endif