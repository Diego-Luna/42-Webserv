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
	string	writeBody(int fdOut);

public:
	~CGI();
	CGI(Req &req_);

	Req		&req;
	string	exec();

	// should probably be set in Req, and accessed through it.
	Server			m_server;
};

/******************
 * ORIGINAL VERSION BELOW
*******************/

// #include "request/Req.hpp"
// #include "../Includes/Parsing.hpp"

// class CGI {

// public:
// 	CGI(Req &req_);
// 	~CGI();

// public:

// 	void m_setEnv();

// 	std::string	m_scriptGet();
// 	std::string m_httpRequestGet();
// 	std::string m_pathInfoGet();
// 	std::string m_queryStringGet();
// 	std::string m_headerGet(std::string header_);

// 	void	exec();

// 	Req& 					_req;
// 	std::vector<std::string>	_env;
	
// 	Server			m_server;


// };

#endif