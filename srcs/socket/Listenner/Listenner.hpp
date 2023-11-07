#ifndef LISTENNER_HPP
#define LISTENNER_HPP


#include "../Csocket.hpp"
#include "../../server/Location.hpp"
#include <poll.h>
# define MAX_CLIENT SOMAXCONN


class listenner : public Csocket
{
public:
	listenner();
	listenner(u_int32_t port, Location &location);
	~listenner();
	void run(void);
private:
	Location _location;
	pollfd fds[MAX_CLIENT];
	u_int32_t n_fd;
	void init(u_int32_t port);

};


#endif