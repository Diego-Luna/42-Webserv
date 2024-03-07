
#include "Response.hpp"

Response::Response(Req &Req_)
 : _Req(Req_)
{
	router(_Req.get_status_code());
}

Response::Response(Req &Req_, string responseBody_)	// Used by CGI
 : _Req(Req_), _responseBody(responseBody_)
{
	router(_Req.get_status_code());
}

Response::~Response(){}

string	Response::CGIHeader(string &header)
{
		header += "Content-Type: text/html";
		header += "\r\n";
		// header += "Content-Length: " + std::to_string(_responseBody.length()); // OG, dropped due to conflict with c98
		header += "Content-Length: ";
		header += size_tToString(_responseBody.length());
		// header += "\r\n";
		// header += "Referer: http://localhost:3000/form.html";
		header += "\r\n\r\n";
		header += _responseBody;
		header += "\r\n";
		return header;
}

string	Response::uploadHeader(string &header)
{
	header += "Content-Type: " + _Req.env["CONTENT_TYPE"];
	header += "\r\n";
	std::fstream htmlFile(UPLOAD_SUCCESS);
	if (!htmlFile.is_open())
	{
		std::cerr << "error opening file: " << UPLOAD_SUCCESS << endl;
		_Req.set_status_code(INTERNAL_SERVER_ERROR);
		return makeErrorHeader();
	}
	string line;
	while (std::getline(htmlFile, line))
		_responseBody += line + "\r\n";
	htmlFile.close();
	header += "Content-Type: text/html";
	header += "\r\n";
	// header += "Content-Length: " + std::to_string(_responseBody.length());
	header += "Content-Length: ";
	header += size_tToString(_responseBody.length());
	header += "\r\n\r\n";
	header += _responseBody;
	header += "\r\n";
	return header;
}

string	Response::makeHeader()
{
	string header = "";

	header += _Req.env["SERVER_PROTOCOL"] + " ";
	// header += std::to_string(_Req.get_status_code()) + " ";
	header += size_tToString(_Req.get_status_code()) + " ";
	header += message_status_code(_Req.get_status_code()) + "\r\n";
	header += "Connection: " + _Req.env["CONNECTION"];

	if (_Req.getIsCGI() && _Req.get_status_code() == OK) {
		header = CGIHeader(header);
	}
	else if (_Req._isUpload && _Req.get_status_code() == OK) {
		// cout <<  "................> if 2" << endl;
		header = uploadHeader(header);
	}
	else if (_Req.get_status_code() == OK) {

		std::fstream htmlFile(_Req.env["FILE_NAME"].c_str());
		if (!htmlFile.is_open())
		{
			std::cerr << "error opening file: " << _Req.env["FILE_NAME"] << endl;
			_Req.set_status_code(INTERNAL_SERVER_ERROR);
			return makeErrorHeader();
		}
		string line;
		while (std::getline(htmlFile, line))
			_responseBody += line + "\r\n";
		htmlFile.close();
		header += "Content-Type: " + _Req.env["CONTENT_TYPE"];
		header += "\r\n";
		// header += "Content-Length: " + std::to_string(_responseBody.length());
		header += "Content-Length: ";
		header += size_tToString(_responseBody.length());
		header += "\r\n\r\n";
		header += _responseBody;
		header += "\r\n";
	}
	return header;
}

void	Response::router(u_int16_t statusCode)
{
	switch (statusCode / 100)
	{
		case 2:
			switch (statusCode)
			{
				case OK:
				case OK_CREATED:
				case OK_NO_CONTENT:
					_header = makeHeader();
					_Req.responseString = _header;
					break;
			}
			break;
		case 4:
		case 5:
			_header = makeErrorHeader();
			_Req.responseString = _header;
			break;
	}
}

string	Response::makeErrorHeader()
{
	string header = "";

	header += _Req.env["SERVER_PROTOCOL"] + " ";
	// header += std::to_string(_Req.get_status_code()) + " ";
	header += size_tToString(_Req.get_status_code());
	header += message_status_code(_Req.get_status_code()) + "\r\n";

	string errorPage = findErrorPage(_Req.get_status_code());
	std::fstream htmlFile(errorPage.c_str());
		if (!htmlFile.is_open())
		{
			std::cerr << "error opening file: " << errorPage << endl;
			return "";
		}
		string line;
		while (std::getline(htmlFile, line))
			_responseBody += line + "\r\n";
		htmlFile.close();
		header += "Content-Type: " "text/html";
		header += "\r\n";
		// header += "Content-Length: " + std::to_string(_responseBody.length());
		header += "Content-Length: ";
		header += size_tToString(_responseBody.length());
		header += "\r\n\r\n";
		header += _responseBody;
		header += "\r\n";
		return header;
}

string	Response::findErrorPage(u_int16_t statusCode)
{
	string	errorRoot = _Req.getRoot() + "/ErrorPages/";
	string	fileName;
	switch (statusCode)
	{
		case 204:
			fileName = errorRoot + "204noContent.html";
			return fileName;
		case 404:
			fileName = errorRoot + "404notFound.html";
			return fileName;
		case 400:
			fileName = errorRoot + "400badRequest.html";
			return fileName;
		default:
			fileName = errorRoot + "500internalServerError.html";
			return fileName;
	}
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
	if (code == HTTP_VERSION_NOT_SUPPORTED)
		return (MESSAGE_VERS_NOT_SUPPORTED);
	else
		return (MESSAGE_INTERNAL_SERVER_ERROR);
}

void Response::setBody(const std::string& body) {
    _responseBody = body;
}
