#include "Req.hpp"

/*************************************************************************
		CANNONICAL FORM REQUIREMENTS
**************************************************************************/
Req::Req(std::string HTTP_Req, const int fd, Location &location, listenner &listenner_)
	:_location(location), _http_Req(HTTP_Req), _client(fd), _listenner(listenner_)
{
	_error = false;
	_isUpload = false;
	envCGIExecve = NULL;
	if (HTTP_Req.length() < 1)
		fatal("Bad HTTP REQUEST");
	_ReqStream.str(_http_Req);
	if (!_ReqStream.good())
		fatal ("failed to create request string stream");
	parseHeader();
	
	if (_isUpload == true && _error == false)
	{
						cout << "found a valid upload" << endl;
						parseUpload();
	}

	else if (_isCGI && _error == false)
	{
		CGI	Cgi(*this);
	} else {
		Response response(*this);
	}
}
			// MISSING:operator=overload

Req::~Req()
{
	if (envCGIExecve)
	{
		size_t	mapSize = env.size();
		for (size_t i = 0; i < mapSize; i++)
		{
			delete[] envCGIExecve[i];
		}
		delete[] envCGIExecve;
	}
}

/**************************************************************************
		CGI PREP
**************************************************************************/

void	Req::_makeEnv(void)
{
	_populateEnv(string("Host"));
	_populateEnv(string("User-Agent"));
	_populateEnv(string("Content-Length"));
	_populateEnv(string("Accept"));
	_populateEnv(string("Accept-Language"));
	_populateEnv(string("Connection"));
	_buildEncoded();
	env["CONTENT_TYPE"] = getContentType(_extension);
	env["SERVER_PROTOCOL"] = _protocol;
	env["FILE_NAME"] = _decodeURI(_fileName);
	env["PATH_INFO"] = _decodeURI(_pathInfo);
	if (!_querryString.empty())
		env["QUERRY_STRING"] = _decodeURI(_querryString);
	//     _env.push_back("SERVER_PORT=" + std::to_string(m_server.get_ports()[0]));
}

string	Req::getContentType(string &extension)
{
	for (std::vector<std::pair<string, string> >::iterator it = mime.begin(); it != mime.end(); it++)
	{
		if (extension == it->first)
			return it->second;
	}
	set_status_code(BAD_REQUEST);
	_error = true;
	return "";
}


void	Req::_buildEncoded()
{
	_encoded["%20"] = " ";
	_encoded["%21"] = "!";
	_encoded["%24"] = "$";
	_encoded["%26"] = "&";
	_encoded["%27"] = "'";
	_encoded["%28"] = "(";
	_encoded["%29"] = ")";
	_encoded["%2A"] = "*";
	_encoded["%2B"] = "+";
	_encoded["%2C"] = ",";
	_encoded["%2F"] = "/";
	_encoded["%3A"] = ":";
	_encoded["%3B"] = ";";
	_encoded["%3D"] = "=";
	_encoded["%3F"] = "?";
	_encoded["%40"] = "@";
	return;
}

string	Req::_decodeURI(string str)
{
	std::map<string, string>::iterator	it = _encoded.begin();

	while (it != _encoded.end())
	{
		size_t pos = str.find(it->first);
		while (pos != string::npos)
		{
			str.replace(pos, it->first.length(), it->second);
			pos = str.find(it->first);
		}
		it++;
	}
	string::iterator	it2 = str.begin();
	while (it2 != str.end())
	{
		if (*it2 == '%')
		{
			string hex = str.substr(it2 - str.begin(), 3);
			if (hex != "%25")
			{
				set_status_code(BAD_REQUEST);
				_error = true;
			}
			str.replace(it2 - str.begin(), 3, "%");
		}
		it2++;
	}
	return str;
}

void	Req::_makeExecveEnv()
{
	size_t	mapSize = env.size();
	envCGIExecve = new char*[mapSize + 1];
	envCGIExecve[mapSize] = NULL;	// null-terminating it here, but if uncessary, remove this line and the +1 in the line above

	size_t	i = 0;
	std::map<string, string>::iterator it = env.begin();
	while(it != env.end())
	{
		envCGIExecve[i] = new char[it->first.size() + it->second.size() + 4];	// +4 -> 3 for " = " + Null termination
		std::strcpy(envCGIExecve[i], (it->first + " = " + it->second).c_str());
		i++;
		it++;
	}
}

void	Req::_populateEnv(string var)
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
		env[_formatStringEnvCGI(var)] = tmp;
	}

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

/*************************************************************************
		VALIDATION
**************************************************************************/


void	Req::_validate()
{
		// checks for invalid characters
	if (!_allValidCharsURI(_fileName))
	{
		perror("invalid Character in File Name");
		set_status_code(BAD_REQUEST);
		_error = true;
		return;
	}
	if (!_allValidCharsURI(_pathInfo))
	{
		perror("invalid Character in Path Info");
		set_status_code(BAD_REQUEST);
		_error = true;
		return;
	}
	if (!_allValidCharsURI(_querryString))
	{
		perror("invalid Character in Querry String");
		set_status_code(BAD_REQUEST);
		_error = true;
		return;
	}
		// checks if files are accessible
	if (access(_fileName.c_str(), F_OK) != 0)
	{
		std::cerr << "filename not accessible" << endl;
		set_status_code(NOT_FOUND);
		_error = true;
		return;
	}
	if (_pathInfo != "")
	{
		if (access(_pathInfo.c_str(), F_OK) != 0)
		{
			perror("Path Info file not found");
			set_status_code(BAD_REQUEST);
			_error = true;
			return;
		}
	}
	if (_validPort(env["HOST"]) == false)
	{
		set_status_code(BAD_REQUEST);
		_error = true;
	}
	set_status_code(OK);

}

bool	Req::_validPort(string host)
{
	size_t	digitPos = host.find_first_of("0123456789");
	if (digitPos == string::npos)
		return false;
	string tmp = host.substr(digitPos);
	if (std::stoi(tmp) == _listenner.portNumber)
		return true;
	else
		return false;
}


bool	Req::_allValidCharsURI(string str)
{
	string::iterator	it = str.begin();
	while (it != str.end())
	{
		if (!_isValidCharURI(static_cast<uint8_t>(*it)))
		{
			return false;
		}
		
		it++;
	}
	return true;
}

bool	Req::_isValidCharURI(uint8_t ch)
{
	if ((ch >= '#' && ch <= ';') || (ch >= '?' && ch <= '[') || (ch >= 'a' && ch <= 'z') ||
       ch == '!' || ch == '=' || ch == ']' || ch == '_' || ch == '~')
        return (true);
	return false;
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

bool	Req::getIsCGI() const
{
	return this->_isCGI;
}

string	Req::getBody()const
{
	return this->_body;
}

string	Req::getRoot()const
{
	return _location.get_root();
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
	cout << "_fileName: ";
	cout << _fileName << endl;
	if (_isCGI)
	{
		cout << "CGI: true"	<< endl;
		cout << "\nprinting env" << endl;
		for (std::map<string, string>::iterator it = env.begin(); it != env.end(); it++)
		{
			cout << it->first << ": " << it->second << endl;
		}
		cout << "\nprinting char** CGIenv" << endl;
		for(size_t i = 0; i < env.size(); i++)
		{
			printf("%s\n", envCGIExecve[i]);
		}
	}
	else
		cout << "NO CGI" << endl;
	cout << "END OF PRINT" << endl;
}
