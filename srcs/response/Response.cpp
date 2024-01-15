
#include "Response.hpp"

Response::Response(Req &Req_)
 : _Req(Req_)
{
	// _responseBody = "";
}

Response::Response(Req &Req_, string responseBody_)
 : _Req(Req_), _responseBody(responseBody_)
{
	string header = makeHeader();

			cout << "printing response header:\n" << header << endl;
}

Response::~Response()
{
}

string	Response::makeHeader()
{
	string header = "";

	header += _Req.env["SERVER_PROTOCOL"] + " ";
	header += std::to_string(_Req.get_status_code()) + " ";
	header += message_status_code(_Req.get_status_code()) + "\r\n";
	header += "Content-Type: " + _Req.env["CONTENT_TYPE"] + "\r\n";
	header += "Content Length: " + _Req.env["CONTENT_LENGTH"] + "\r\n\r\n";
	if (_Req.getIsCGI())
	{
		header += _responseBody;
	}
	return header;
}

const std::string Response::message_status_code(u_int16_t code)
{
	if (code == OK)
		return (MESSAGE_OK);
	if (code == OK_CREATED)
		return (MESSAGE_OK_CREATED);
	if (code == OK_NO_CONTENT)
		return (MESSAGE_OK_NO_CONTENT);
	if (code == MOVED_TO_NEW_URL)
		return (MESSAGE_MOVED_TO_NEW_URL);
	if (code == NOT_MODIFIED)
		return (MESSAGE_NOT_MODIFIED);
	if (code == BAD_REQUEST)
		return (MESSAGE_BAD_REQUEST);
	if (code == UNAUTHORIZED)
		return (MESSAGE_UNAUTHORIZED);
	if (code == NOT_FOUND)
		return (MESSAGE_NOT_FOUND);
	else
		return (MESSAGE_INTERNAL_SERVER_ERROR);	
}