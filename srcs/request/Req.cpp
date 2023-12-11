#include "Req.hpp"

/*************************************************************************
		CANNONICAL FORM REQUIREMENTS
**************************************************************************/
Req::Req(std::string HTTP_Req, const int fd, Location &location)
	: _client(fd), _location(location), _http_Req(HTTP_Req)
{
	if (HTTP_Req.length() < 1)
		fatal("Bad HTTP REQUEST");
	_ReqStream.str(_http_Req);
	if (!_ReqStream.good())
		fatal ("failed to create request string stream");
	parseHeader();

}
			// MISSING: copy, operator=overload

Req::~Req()
{
	// size_t	mapSize = envCGI.size();
	// for (size_t i = 0; i < mapSize; i++)
	// {
	// 	delete[] envCGIExecve[i];
	// }
	// delete[] envCGIExecve;
}

/**************************************************************************
		PARSING
**************************************************************************/
// sets _header and variable. Checks if it's a CGI request. determines which function(GET POST DELETE).
void	Req::parseHeader(void)
{
	string line;
	while (std::getline(_ReqStream, line) && !line.empty())
		_header += line + '\n';
	while (std::getline(_ReqStream, line) && !line.empty())
		_body += line + '\n';
										// cout << "print header: \n\n" << _header << endl << endl;
	parseFirstLine();
	if (_isCGI)
		_makeEnvCGI();
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
	_querryString = _getQuerryString(line);
	_isCGI = _checkCGI(line);
}

string	Req::_getQuerryString(string &line)
{
	size_t querryStringStart = line.find("?");
	if (querryStringStart == string::npos)
		return "";
	return (line.substr(querryStringStart, string::npos));
}

bool	Req::_checkCGI(string &firstLine)
{
	size_t pos	= firstLine.find(".cgi");
	if (pos == string::npos)
		return false;
	size_t repeatCheck = firstLine.rfind(".cgi");
	if (pos != repeatCheck)
	{
		fatal("Invalid HTTP Request");
		return false;
	}
	return true;
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
	string::iterator it = line.begin() + line.find('/');
	size_t	extensionEnd = _findExtensionEnd(line);
	if (extensionEnd == string::npos)
		fatal("invalid HTTP Request: resource path");
	_scriptName = line.substr(it - line.begin(), extensionEnd - (it - line.begin()));
	it = line.begin() + extensionEnd;
	while (*it != ' ' && it != line.end())
	{
		_pathInfo += *it;
		it++;
	}
	return true;
}

	// assumes files can only be either .hmtl or .cgi
size_t	 Req::_findExtensionEnd(string &line)
{
	size_t	extentionPos;
	extentionPos = line.find(".hmtl/");
	if (extentionPos != string::npos)
		return extentionPos + 6;
	extentionPos = line.find(".cgi/");
	if (extentionPos != string::npos)
		return extentionPos + 5;
	return string::npos;
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

/**************************************************************************
		CGI PREP
**************************************************************************/


	// add  a few new vars:
	//	script name
	//	path info: 

void	Req::_makeEnvCGI(void)
{
	_populateEnvCGI(string("Host"));
	_populateEnvCGI(string("User-Agent"));
	_populateEnvCGI(string("Content-Type"));
	_populateEnvCGI(string("Content-Length"));
	_populateEnvCGI(string("Accept"));
	_populateEnvCGI(string("Accept-Language"));
	_populateEnvCGI(string("Connection"));
	envCGI["METHOD"] = _method;		// faulty
	if (_header.find("HTTP/1.0"))
		envCGI["VERSION"] = "HTTP/1.0";
	else if (_header.find("HTTP/1.1"))
		envCGI["VERSION"] = "HTTP/1.1";
	envCGI["SCRIPT_NAME"] = _scriptName;
	envCGI["PATH_INFO"] = _pathInfo;
	if (!_querryString.empty())
		envCGI["QUERRY_STRING"] = _querryString;
	// MISSING: path name/file name? whats the variable? Could be set prior during verification.
	_makeExecveEnv();
}

void	Req::_makeExecveEnv()
{
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

void	Req::_populateEnvCGI(string var)
{
	string				tmp;
	string::iterator	it;
	
	if (!_isValidVariable(var))
		return;
	size_t pos = _header.find(var) + var.length() + 2; // +2 -> accounts for the ": "
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
								// cout << "TMP!!!! : " << tmp << endl;

	return;
}

bool	Req::_isValidVariable(string &var)
{
	if (_header.find(var) == string::npos)
		return false;
	string::iterator it = _header.begin() + _header.find(var) + var.length();	// moves it to the last char of 
	if (it >= _header.end())
		return false;
	if (*it != ':')
		return false;
	it++;
	if(*it != ' ')
		return false;
	it++;
	if(std::isgraph(*it) == false)
		return false;
	return true;
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

/**************************************************************************
		GETTERS / SETTERS
**************************************************************************/

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

std::string	Req::getHttpString()const
{
	return this->_http_Req;
}


/**************************************************************************
		PRINTING / TESTING
**************************************************************************/
void	Req::printReq() {
	
	cout << "\nPRINTING REQUEST\n" << endl;
	cout << "FULL http Req:\n";
	cout << _http_Req << endl << endl;
	cout << "header:\n";
	cout << _header << endl << "end of header" << endl;
	cout << "body: ";
	cout << _body << endl;
	cout << "_scriptName: ";
	cout << _scriptName << endl;
	if (_isCGI)
	{
		cout << "CGI: true"	<< endl;
		cout << "\nprinting envCGI" << endl;
		for (std::map<string, string>::iterator it = envCGI.begin(); it != envCGI.end(); it++)
		{
			cout << it->first << ": " << it->second << endl;
		}
		cout << "\nprinting char** CGIenv" << endl;
		for(size_t i = 0; i < envCGI.size(); i++)
		{
			printf("%s\n", envCGIExecve[i]);
		}
	}
	else
		cout << "NO CGI" << endl;
	cout << "END OF PRINT" << endl;
}

/**************************************************************************
		UNKNOWN OR DEPRECATED
**************************************************************************/

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


