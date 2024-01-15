#pragma once

#include "../request/Req.hpp"

class	Req;

class Response
{
private:
	Response();
	Response(Response &original);
	Req	&_Req;
	string	_responseBody;
	string	makeHeader();
	
public:
	Response(Req &Req_);
	Response(Req &Req_, string responseBody_);
	~Response();
	string	header;
	
};

Response::Response(Req &Req_)
 : _Req(Req_)
{
	// _responseBody = ""; 
	
}

Response::Response(Req &Req_, string responseBody_)
 : _Req(Req_), _responseBody(responseBody_)
{
	
	
}

Response::~Response()
{
}

string	Response::makeHeader()
{
	


}