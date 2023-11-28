#include "Req.hpp"





				// depracated due to not using fd
				// might be creating the body for the response
void Req::body_creation(void)
{
	// if (!this->file.is_open())
	// {
	// 	if (this->status_code == 404)
	// 		this->file.open(this->file_name);
	// 	else if (this->status_code == 400)
	// 		this->file.open(this->file_name);
	// 	if (!this->file.is_open())
	// 		fatal("open");
	// }
	// this->body.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	// file.close();
}

Req::Req(std::string HTTP_Req, const int fd, Location &location)
	: _client(fd), _location(location), http_Req(HTTP_Req)
{
	if (HTTP_Req.length() < 1)
		fatal("Bad HTTP REQUEST");
	_ReqStream.str(http_Req);
	if (!_ReqStream.good())
		fatal ("failed to create request string stream");
	parseHeader();
	// setBody();
}

// sets _header and variable. Checks if it's a CGI request. determines which function(GET POST DELETE).
void	Req::parseHeader(void)
{
	string line;
	while (std::getline(_ReqStream, line) && !line.empty())
		_header += line + '\n';
										// cout << "print header: \n\n" << _header << endl << endl;
	if (_header.find("CGI: true") != string::npos)		// can be replaced for any other way to determine cgi. file .cgi for instance
		_isCGI = true;
	else
		_isCGI = false;
	parseFirstLine();
}

	// checks if the first line is valid, which method is called and if the version is correct.
void	Req::parseFirstLine(void)
{
	std::stringstream	stream(_header);
	string				line;

	std::getline(stream, line);
	if (_hasMultipleMethods(line))
		fatal ("Invalid HTTP Request");


}


bool	Req::_hasMultipleMethods(const string &line)
{
	int	count = 0;

	if (line.find("GET") != string::npos)
	{
		if (line.find("GEt") != line.rfind("GET"))
			return true;
		count++;
	}
	if (line.find("POST") != string::npos)
	{
		if (line.find("POST") != line.rfind("POST"))
			return true;
		count++;
	}
	if (line.find("DELETE") != string::npos)
	{
		if (line.find("DELETE") != line.rfind("DELETE"))
			return true;
		count++;
	}
	if (count != 1)
		return true;
	else
		return false;
}



u_int16_t Req::get_status_code() const
{
	return this->status_code;
}
void Req::set_status_code(u_int16_t statusCode_)
{
	this->status_code = statusCode_;
}

std::string	Req::get_header()const
{
	return this->_header;
}

std::string	Req::getHttpString(void)
{
	return this->http_Req;
}

void	Req::printReq() {
	
	cout << "\nPRINTING REQUEST\n" << endl;
	cout << "status line: ";
	cout << status_line << endl;
	cout << "header: ";
	cout << _header << endl;
	cout << "body: ";
	cout << _body << endl;
	cout << "file_name: ";
	cout << file_name << endl;
	cout << "http Req: ";
	cout << http_Req << endl;
	cout << "status code: ";
	cout << status_code << endl;
	cout << "END OF PRINT" << endl;
}


Req::~Req()
{
}