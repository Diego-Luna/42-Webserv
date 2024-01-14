#include "Client.hpp"
#include "../Listenner/Listenner.hpp"



Client::Client(listenner &_server)
{
	this->fd_socket = accept(_server.getfd(), (struct sockaddr*)&this->addr, &this->addr_size);
	if (fd_socket == -1)
		fatal("accept");
}

Client::Client(int fd)
{
	this->fd_socket = fd;	
}

Client::~Client()
{}

Client::Client(){};
