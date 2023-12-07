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
private:
	Client();
};

#endif