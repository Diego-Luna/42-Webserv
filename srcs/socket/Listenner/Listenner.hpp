#ifndef LISTENNER_HPP
#define LISTENNER_HPP


#include "../Csocket.hpp"
#include "../../server/Location.hpp"
#include <vector>
#include <iostream>
#include <sstream>
#include <poll.h>

using std::string;

# define MAX_CLIENT SOMAXCONN

class Server;

class listenner : public Csocket
{
public:
	listenner();
	listenner(u_int32_t port, Location &location, std::string host);
	int		portNumber;
	~listenner();
	void run(Server _server);
	void internalError(int clientFd);
private:
	Location _location;
	pollfd fds[MAX_CLIENT];
	u_int32_t n_fd;
	void init(u_int32_t port, std::string host);
	
	// chunk handling
	bool	isChunked(const string &httpRequest);
	bool	isChunkTest(const string &httpRequest);
	bool	isMulti(const string &httpRequest);
	string	unchunk(const string &receivedData);
	string	trimLine(string &line);
	std::vector<string> _chunks;

};

#endif