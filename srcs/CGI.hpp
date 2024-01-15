#pragma once
// #include "../Includes/Parsing.hpp"

#include "request/Req.hpp"
#include "response/Response.hpp"
#include "sys/wait.h"
#include <cstdio>

using std::string;
class Req;
class Response;

#define BUFFER_SIZE 1024


class CGI {
private:
	CGI();
	CGI(CGI &original);

public:
	~CGI();
	CGI(Req &req_);

	Req		&req;
	string	exec();

	// should probably be set in Req, and accessed through it.
	Server			m_server;
};
