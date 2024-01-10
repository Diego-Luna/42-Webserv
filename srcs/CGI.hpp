#ifndef CGI_HPP
#define CGI_HPP


#include "request/Req.hpp"
#include "../Includes/Parsing.hpp"
#include "sys/wait.h"
#include <cstdio>

#define BUFFER_SIZE 1024

class CGI {

private:
	CGI();

public:
	~CGI();
	CGI(Req &req_);

	void	exec();

	Req& 					req;

	// should probably be set in Req, and accessed through it.
	Server			m_server;
};

#endif