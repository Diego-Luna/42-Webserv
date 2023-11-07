#ifndef CLIENT_HPP
#define CLIENT_HPP


#include "../Csocket.hpp"


class listenner;

class client : public Csocket
{
public:
	client(listenner &_server);
	client(int fd);
	~client();
private:
};

#endif