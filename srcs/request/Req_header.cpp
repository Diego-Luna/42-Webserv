#include "Req.hpp"

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
	_makeEnvCGI();
	_validate();
	if (_isCGI)
		_makeExecveEnv();
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
	_isCGI = _checkCGI(_pathInfo);
}

string	Req::_getQuerryString(string &line)
{
	size_t querryStringStart = line.find("?");
	if (querryStringStart == string::npos)
		return "";
	string::iterator	it = line.begin() + querryStringStart;
	string	querryString;
	while (*it != ' ' && it != line.end())
	{
		querryString += *it;
		it++;
	}
	return querryString;
}

		// for now only check files for .cgi extensions. Could Change to check for .py or
		// which other script extensions we would need.
bool	Req::_checkCGI(string &pathInfo)
{
	if (pathInfo == "")
		return false;
	size_t pos	= pathInfo.find(".cgi");
	if (pos == string::npos)
		return false;
	size_t repeatCheck = pathInfo.rfind(".cgi");
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
	string::iterator it = line.begin() + line.find(' ') + 1;
	size_t	extensionEnd = _findExtensionEnd(line, ".html");
	if (extensionEnd == string::npos)
		fatal("invalid HTTP Request: resource path");
	_fileName = line.substr(it - line.begin(), extensionEnd - (it - line.begin()));
	it = line.begin() + extensionEnd;
	if (*it == '/')
	{
		it++;
		while (*it != ' ' && it != line.end() && *it != '?')
		{
			_pathInfo += *it;
			it++;
		}
	}
	return true;
}

size_t	 Req::_findExtensionEnd(string &line, const string &extension)
{
	size_t	extensionPos;
	extensionPos = line.find(extension);
	if (extensionPos != string::npos)
		return extensionPos + extension.length();
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
		UNKNOWN OR DEPRECATED
**************************************************************************/

std::string Req::cType( void )
{
	std::string tmp;

	size_t pos = this->_fileName.find('.');
	if (pos == std::string::npos)
		return(std::string("application/octet-stream"));
	tmp = this->_fileName.substr(pos);
	for (size_t i = 0; i < mime.size(); i++)
	{
		if (tmp == mime[i].first)
			return (mime[i].second);
	}
	return (std::string("application/octet-stream"));
}

				// unable to test right now due to funcitons needind to be a socket and not just an fd
				// I think this method is actually creating the response _header, as the request _header is 
				// already given to use
void Req::header_creation(void)
{
	this->_header = "Host: " + std::string(inet_ntoa(_client.getAddr().sin_addr)) + ":" + std::to_string(ntohs(_client.getAddr().sin_port)) + "\r\n";
	if (this->methode == &Req::getFonc || this->status_code == 404 || this->status_code == 400)
	{
		this->_header += "Content-Type: " + cType() + "; charset=UTF-8\r\n"; // peut PEUT ËTRE causé un problme (utf-8 etc.)
		this->_header += "Content-Length: " + std::to_string(this->_body.length()) + "\r\n";
	}

	// 				User-Agent
	this->_header += "\r\n";
}
