#ifndef CGI_HPP
#define CGI_HPP

// #include "../Includes/Parsing.hpp"

#include "request/Req.hpp"
#include "response/Response.hpp"
#include "sys/wait.h"
#include <cstdio>
#include <string>
#include <iostream>
#include <poll.h>

using std::string;
class Req;
class Response;

#define BUFFER_SIZE 1024

class CGI {
private:
	CGI();
	CGI(CGI &original);
	string	makeBody(int fdOut);
	
public:
	~CGI();
	CGI(Req &req_);

	Req		&req;
	string	exec();

	Server			m_server;
};
#endif