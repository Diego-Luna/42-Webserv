#pragma once

#include "../request/Req.hpp"
#include <string>

using std::string;

class	Req;

class Response
{
private:
	Response();
	Response(Response &original);
	Req	&_Req;

	string			_responseBody;
	string			_header;
	string			makeHeader();
	const string	message_status_code(u_int16_t code);

public:
	Response(Req &Req_);
	Response(Req &Req_, string responseBody_);
	~Response();
	string	header;
	
};
