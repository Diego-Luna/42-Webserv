
#include "Response.hpp"

Response::Response(Req &Req_)
 : _Req(Req_)
{
		_header = makeHeader();
		
					_Req.responseString = _header;
		
					cout << "writing header to client:\n" << _header << endl;
		writeToClient();
}

Response::Response(Req &Req_, string responseBody_)	// Used by CGI
 : _Req(Req_), _responseBody(responseBody_)
{
	_header = makeHeader();
	
				_Req.responseString = _header;
				cout << "writing header to client:\n" << _header << endl;
	writeToClient();
}

Response::~Response()
{
				// cout << "response destructor called" << endl;
}


void	Response::writeToClient()
{
	struct	pollfd	pollContext;
	pollContext.fd = _Req._client.getfd();
	pollContext.events = POLLOUT;
	int	pollReturn = poll(&pollContext, 1, POLL_INF_TIME);	// could possibly se time to a very high value so it wouldnt block?
	
	if (pollReturn > 0)
	{
		if (pollContext.revents & POLLOUT) {
			int	bytesWritten = write(_Req._client.getfd(), _header.c_str(), _header.length());
			if (bytesWritten == -1)
			{
				std::cerr << "error writting to socket" << endl;
				// throw exception?
			}
						cout << "bytes written to client: " << bytesWritten << endl;
		}
	} else {
		// poll error 
		// throw exception?
	}
}

string	Response::makeHeader()
{
	string header = "";

	header += _Req.env["SERVER_PROTOCOL"] + " ";
	header += std::to_string(_Req.get_status_code()) + " ";
	header += message_status_code(_Req.get_status_code()) + "\r\n";

	if (_Req.getIsCGI() && _Req.get_status_code() == OK)
	{
		header += "Content-Type: " "text/html";
		header += "\r\n";
		header += "Content-Length: " + std::to_string(_responseBody.length());
		header += "\r\n\r\n";
		header += _responseBody;
		header += "\r\n";

	} else if (_Req.get_status_code() == OK) {
		std::fstream htmlFile(_Req.env["FILE_NAME"]);
		if (!htmlFile.is_open())
		{
			std::cerr << "error opening file: " << _Req.env["FILE_NAME"] << endl;
			return "";
		}
		string line;
		while (std::getline(htmlFile, line))
			_responseBody += line + "\r\n";
		htmlFile.close();
		header += "Content-Type: " + _Req.env["CONTENT_TYPE"];
		header += "\r\n";
		header += "Content-Length: " + std::to_string(_responseBody.length());
		header += "\r\n\r\n";
		header += _responseBody;
		header += "\r\n";
		return header;
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
