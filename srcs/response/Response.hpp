#pragma once

#include "../request/Req.hpp"
#include "../../Includes/Utils.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <poll.h>

#define	UPLOAD_SUCCESS "data/www/Pages/uploadSuccessful.html"

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
	void			router(u_int16_t statusCode);
	string			makeErrorHeader();
	string			findErrorPage(u_int16_t statusCode);
	string			CGIHeader(string &header);
	string			uploadHeader(string &header);
public:
	Response(Req &Req_);
	Response(Req &Req_, string responseBody_);
	~Response();
	string	header;

	void setBody(const std::string& body);
};
