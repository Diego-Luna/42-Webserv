#include "Req.hpp"

/**************************************************************************
		PARSING
**************************************************************************/
// sets _header and variable. Checks if it's a CGI request. determines which function(GET POST DELETE).
void	Req::parseHeader(void)
{
	string line;

	while (std::getline(_ReqStream, line) && !line.empty() && (line.find_first_not_of(" \t\r\n") != string::npos))
		_header += line + '\n';

	while (std::getline(_ReqStream, line) && !line.empty())
		_body += line + '\n';

	parseFirstLine();

	if (_isUpload == true && _error == false)
		return;

	_makeEnv();

	if (_error == false)
		_validate();
	if (_isCGI && _error == false)
		_makeExecveEnv();
}

	// checks if the first line is valid, which method is called and if the version is correct.
void	Req::parseFirstLine(void)
{
	std::stringstream	stream(_header);
	string				line;

	std::getline(stream, line);
	if (!_validMethod(line))
	{
		set_status_code(BAD_REQUEST);
		_error = true;
		return;
	}
	if (!_validPath(line))
	{
		set_status_code(NOT_FOUND);
		_error = true;
		return;
	}
	if (!_validVersion(line))
	{
		set_status_code(BAD_REQUEST);
		_error = true;
		return;
	}
	if (_isUpload == true && _error == false)
		return;
	if (_error == false)
	{
		_querryString = _getQuerryString(line);
		_isCGI = _checkCGI(_fileName);
	}
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

		// for now only check files for .py extensions. Could Change to check for .py or
		// which other script extensions we would need.
		// possibly check if it is in the /CGI/bin folder
bool	Req::_checkCGI(string &fileName)
{
	if (fileName == "")
		return false;
	size_t pos	= fileName.find(".py");
	if (pos == string::npos)
		return false;
	size_t repeatCheck = fileName.rfind(".py");
	if (pos != repeatCheck)
	{
		_error = true;
		set_status_code(BAD_REQUEST);
		return false;
	}
	if (_pathInfo != "")
		_pathInfo = fileName;
	return true;
}

	// currently doesn't account for spaces. Request must contain
	// exactly "HTTP/{VERSION}" or be considered invalid.
bool	Req::_validVersion(string &line)
{
	size_t	ver1 = line.find("HTTP/1.1");
	size_t	ver2 = line.find("HTTP/1.0");
	if ((ver1 == string::npos && ver2 == string::npos) ||
		(ver1 != string::npos && ver2 != string::npos))
		return false;
	if (ver1 != string::npos)
		_protocol = "HTTP/1.1";
	else
		_protocol = "HTTP/1.0";
	return true;
}

bool	Req::_validPath(string &line)
{
	if (line.compare(0, 6, "GET / ") == 0)	// checks for initial index call: "GET / HTTP:1.1"
	{
		_fileName = PATH_TO_INDEX;
		_extension = ".html";
		return true;
	}

	if (line.compare(0, 13, "POST /upload/") == 0)
	{
						// cout << "FOUND UPLOAD, is it valid?" << endl;
		_isUpload = true;
		return true;
	}
	size_t	extensionEnd = _findExtensionEnd(line);
	if (extensionEnd == string::npos)
		return false;
	if (line.at(extensionEnd) != ' ' && line.at(extensionEnd) != '/')
		return false;
	if (_extension == ".py")
		_fileName = PATH_TO_CGI;
	else
		_fileName = _location.get_root();
	string::iterator it = line.begin() + line.find(' ') + 1;
	_fileName += line.substr(it - line.begin(), extensionEnd - (it - line.begin()));
	it = line.begin() + extensionEnd;
	if (*it == '/')	// checks for PATH_INFO
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

	// refarctored to use mim instead of it's own vector
	//	looks for the extension(mime.first) in line. sets _extension if found
	// returns the position in line of the last charcter of extension.
size_t	 Req::_findExtensionEnd(string &line)
{
	string	extension = "";
	for(string::iterator it = line.begin(); it != line.end(); it++)
	{
		if (*it == '.')
		{
			while (*it != ' '){
				extension += *it;
				it++;
			}
			break;
		}
	}
	std::vector<std::pair<std::string, std::string> >::iterator it = mime.begin();
	while (it != mime.end())
	{
		if (it->first == extension)
		{
			_extension = it->first;
			return line.find(_extension) + _extension.length();
		}
		it++;
	}
	return std::string::npos;
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
