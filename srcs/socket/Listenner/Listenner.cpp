
#include "Listenner.hpp"
#include "../../request/Req.hpp"

void listenner::init(u_int32_t port, std::string host)
{
	u_int32_t e = 1;
	this->fd_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_socket == -1)
		fatal("socket");
	if (setsockopt(this->fd_socket, SOL_SOCKET, SO_REUSEADDR, &e, sizeof(e)) < 0)
		fatal("setsockopt");
	this->addr.sin_family = AF_INET;
	this->addr.sin_port = htons(port);
	// this->addr.sin_addr.s_addr = INADDR_ANY;
	this->addr.sin_addr.s_addr = inet_addr(host.c_str());
	if (bind(this->fd_socket, (struct sockaddr *)&this->addr, this->addr_size) < 0)
		fatal("bind");
	if (listen(this->fd_socket, SOMAXCONN) < 0)
		fatal("listen");


 // flagged as error for not multiplying maxclient by its element size
	// memset(this->fds, 0, MAX_CLIENT);
memset(this->fds, 0, MAX_CLIENT * sizeof(this->fds[0]));


	this->fds[0].fd = this->getfd();
	this->fds[0].events = POLLIN;
	this->n_fd = 1;
}

listenner::listenner(u_int32_t port, Location &location, std::string host) : _location(location)
{
	std::cout << "host: " << host << " port :" <<  port << std::endl;
	init(port, host);
}

listenner::listenner()
{
	init(DEFAULT_PORT, "127.0.0.1");
}

listenner::~listenner()
{}

void listenner::run()
{
	if (poll(fds, this->n_fd, 100) < 0)
	{
		fatal("poll");
	}
	for (u_int16_t i = 0; i < this->n_fd; i++)
	{
		if (fds[i].revents & POLLIN)
		{
			if (i == 0)
			{
				this->fds[this->n_fd].fd = client(*this).getfd();
				this->fds[this->n_fd].events = POLLIN;
				this->n_fd++;
			}
			else
			{
				char buffer[0xffff];
				int res = recv(fds[i].fd, buffer, 0xffff - 1, 0);
				if (res < 0) // pt faire une erreur 500 ici si on spam :/
				{
					continue;
				}
				else if (res == 0)
				{
					close(fds[i].fd);  // close est une fonction autorisée!
					fds[i] = fds[n_fd - 1];
					n_fd--;
				}
				else
				{
					std::cout << RED << "[DEBUG] [RECV] : \n" << RESET <<  buffer << std::endl;

					try {
							cout << "calling Request Class" << endl;

						Req x(std::string(buffer), fds[i].fd, this->_location);
						
										// might be buggy because the current version doesnt use getHttpString
						
						// if (send(fds[i].fd, x.getHttpString().c_str(), x.getHttpString().length(), 0) < 0)
						// {
						// 	continue; // même chose quen haut, pt erreur 500, a voir
						// }

						// std::cout << RED << "[DEBUG] [SEND] : \n" << RESET <<  x.getHttpString() << std::endl;
					}
					catch (std::exception &e) {
						std::cout << RED << "[DEBUG] catch: \n" << RESET <<  e.what() << std::endl;
					}
				}
			}
		}
	}
}
