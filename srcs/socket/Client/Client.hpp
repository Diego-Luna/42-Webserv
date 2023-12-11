#ifndef CLIENT_HPP
#define CLIENT_HPP


#include "../Csocket.hpp"


class listenner;

class Client : public Csocket
{
public:
	Client(listenner &_server);
	Client(int fd);
	~Client();
	Client();		// shouldn't be used but can't be put as private due to other clases using it
private:
};

#endif