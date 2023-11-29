#include "Req.hpp"





				// depracated due to not using fd
				// I think this might actuallly be creating the body for the response
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

}

// sets _header and variable. Checks if it's a CGI request. determines which function(GET POST DELETE).
void	Req::parseHeader(void)
{
	string line;
	while (std::getline(_ReqStream, line) && !line.empty())
		_header += line + '\n';
										// cout << "print header: \n\n" << _header << endl << endl;
	if (_header.find("CGI = true") != string::npos)		// can be replaced for any other way to determine cgi. file .cgi for instance
	{
		_isCGI = true;
		_makeEnvCGI();
	}
	else
		_isCGI = false;
	parseFirstLine();
	
}

void	Req::_makeEnvCGI(void)
{
	_populateVarEnvCGI(string("Host"));
	_populateVarEnvCGI(string("User-Agent"));
	_populateVarEnvCGI(string("Querry-String"));
	_populateVarEnvCGI(string("Content-Type"));
	_populateVarEnvCGI(string("Content-Length"));
	_populateVarEnvCGI(string("Accept"));
	_populateVarEnvCGI(string("Accept-Language"));
	_populateVarEnvCGI(string("Connection"));
	envCGI["METHOD"] = _method;		// faulty
	if (_header.find("HTTP/1.0"))
		envCGI["VERSION"] = "HTTP/1.0";
	else if (_header.find("HTTP/1.1"))
		envCGI["VERSION"] = "HTTP/1.1";
	
	// MISSING: path name/file name? whats the variable? Could be set prior during verification.
	
				// cout << "printing envCGI[QUERRY STRING]:\n" << envCGI["QUERRY_STRING"] << endl;
	size_t	mapSize = envCGI.size();

	envCGIExecve = new char*[mapSize + 1];
	envCGIExecve[mapSize + 1] = NULL;	// null-terminating it here, but if uncessary, remove this line and the +1 in the line above
	size_t	i = 0;
	std::map<string, string>::iterator it = envCGI.begin();
	
	while(it != envCGI.end())
	{
		envCGIExecve[i] = new char [it->first.size() + it->second.size() + 4];	// +4 -> 3 for " = " + Null termination
		std::strcpy(envCGIExecve[i], (it->first + " = " + it->second).c_str());
		i++;
		it++;
	}
}

void	Req::_populateVarEnvCGI(string var)
{
	string				tmp;
	string::iterator	it;
	
	size_t pos = _header.find(var);
	if (pos != string::npos)
	{
		it = _header.begin() + pos;
		while (*it != '\n' && it != _header.end())
		{
			tmp += *it;
			it++;
		}
		envCGI[_formatStringEnvCGI(var)] = tmp;
	}
	return;
}


string	Req::_formatStringEnvCGI(string str)
{
	string::iterator	it = str.begin();
	while (it != str.end())
	{
		*it = std::toupper(*it);
		if (*it == '-')
			*it = '_';
		it++;
	}
	return str;
}


	// checks if the first line is valid, which method is called and if the version is correct.
void	Req::parseFirstLine(void)
{
	std::stringstream	stream(_header);
	string				line;

	std::getline(stream, line);
	if (!_validMethod(line))
		fatal("Invalid HTTP Request");
	if (!_validPath(line))
		fatal("Invalid HTTP Request");
	if (!_validVersion(line))
		fatal("Invalid HTTP Request");
}

	// currently doesn't account for spaces. Request must contain
	// exactly "HTTP/{VERSION}" or br considered invalid.
bool	Req::_validVersion(string &line)
{
	size_t	ver1 = line.find("HTTP/1.1");
	size_t	ver2 = line.find("HTTP/1.0");
	if ((ver1 == string::npos && ver2 == string::npos) ||
		(ver1 != string::npos && ver2 != string::npos))
		return false;
	
	return true;
}

bool	Req::_validPath(string &line)
{
	//	checks if the given path is valid:
		//check with Location?
		(void)line;
	return true;
}

bool	Req::_validMethod(const string &line)
{
	int	count = 0;

	if (line.find("GET") != string::npos)
	{
		_method = "GET";
		if (line.find("GET") != line.rfind("GET"))
			return false;
		count++;
	}
	if (line.find("POST") != string::npos)
	{
		_method = "POST";
		if (line.find("POST") != line.rfind("POST"))
			return false;
		count++;
	}
	if (line.find("DELETE") != string::npos)
	{
		_method = "DELETE";
		if (line.find("DELETE") != line.rfind("DELETE"))
			return false;
		count++;
	}
	if (count != 1)
		return false;
	else
		return true;
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
	cout << "printing char** CGIenv" << endl;
	for(int i = 0; envCGIExecve[i] != NULL; i++)
	{
		printf("%s\n", envCGIExecve[i]);
	}
	cout << "END OF PRINT" << endl;
}

Req::~Req()
{
	size_t	mapSize = envCGI.size();
	for (size_t i = 0; i < mapSize + 1; i++)
	{
		delete[] envCGIExecve[i];
	}
	delete[] envCGIExecve;
}