#include "Client.hpp"
#include "../Listenner/Listenner.hpp"



client::client(listenner &_server)
{
	this->fd_socket = accept(_server.getfd(), (struct sockaddr*)&this->addr, &this->addr_size);
	if (fd_socket == -1)
		fatal("accept");
}

client::client(int fd)
{
	this->fd_socket = fd;	
}

client::~client()
{}