#ifndef CGI_HPP
#define CGI_HPP


#include "request/Req.hpp"
#include "../Includes/Parsing.hpp"

class CGI {

public:
	CGI(Req &req_);
	~CGI();

public:

	void m_setEnv();

	std::string	m_scriptGet();
	std::string m_httpRequestGet();
	std::string m_pathInfoGet();
	std::string m_queryStringGet();
	std::string m_headerGet(std::string header_);

	void	exec();

	Req& 					_req;
	std::vector<std::string>	_env;
	Server			m_server;


};



#endif