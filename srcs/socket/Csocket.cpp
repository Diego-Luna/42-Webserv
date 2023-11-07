#include <cstring> 
#include "Csocket.hpp"

Csocket::Csocket() : fd_socket(0)
{
	this->addr_size = sizeof(this->addr);
	memset(&this->addr, 0, this->addr_size);
}

Csocket::~Csocket()
{}

u_int32_t	Csocket::getfd(void)
{
	return (this->fd_socket);
}

sockaddr_in Csocket::getAddr(void)
{
	if (getsockname(this->fd_socket, (struct sockaddr *)&addr, &this->addr_size) < 0)
		fatal("getsockname");
	return (addr);
}