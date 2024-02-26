#include "Req.hpp"

/*************************************************************************
		CANNONICAL FORM REQUIREMENTS
**************************************************************************/
// Req::Req(Server _server, string httpRequest, const int fd, Location &location, listenner &listenner_)
// 	: _location(location), _server(_server), _http_Req(httpRequest), _client(fd), _listenner(listenner_)
// {
	// cout<< "<> hello " <<endl;
	// cout<< "==> _server = [" << _server.get_location_size() << "]"  <<endl;
	// cout<< "==> _server = [" << _server.get_location(0).get_name() << "]"  <<endl;
	// cout<< "==> _server = [" << _server.get_location(1).get_name() << "]"  <<endl;
// }

Req::Req(Server _server, string httpRequest, const int fd, Location &location, listenner &listenner_)
	: _location(location), _server(_server), _http_Req(httpRequest), _client(fd), _listenner(listenner_)
{
	_error = false;
	_isUpload = false;
	envCGIExecve = NULL;
	if (_http_Req.length() < 1)
	{
		_error = true;
		set_status_code(BAD_REQUEST);
	}
	_ReqStream.str(_http_Req);
	if (!_ReqStream.good())
		{
		_error = true;
		set_status_code(INTERNAL_SERVER_ERROR);
	}
	if (_error == false)
		parseHeader();

	if (_isUpload == true && _error == false)
	{
		parseUpload();
		if (_error == false)
			createUploadFile();
		Response response(*this);
		return;
	}
	else if (_isCGI && _error == false)
	{
		CGI	Cgi(*this);
	} else {
		if (_run_location(_extractURL(httpRequest), httpRequest) == false){
			Response response(*this);
		} else {
			status_code = 200;
			Response response(*this, this->_data_file);
		}
	}
}

Req::~Req()
{
	if (envCGIExecve)
	{
		// printReq();
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
	if (std::atoi(tmp.c_str()) == _listenner.portNumber)
		return true;
	else
		return false;
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

/**************************************************************************
		Locacion
**************************************************************************/

// std::string Req::_extractURL(const std::vector<char>& dataVector) {
std::string Req::_extractURL(std::string &dataVector) {
  // Convertir el vector de caracteres a un string
  std::string data(dataVector.begin(), dataVector.end());

  // Buscar el inicio de la petición GET
  size_t start = data.find("GET ");
  if (start == std::string::npos) {
      return ""; // No se encontró la petición GET
  }
  start += 4; // Ajustar para omitir "GET "

  // Buscar el final de la URL (espacio antes de HTTP)
  size_t end = data.find(" ", start);
  if (end == std::string::npos) {
      return ""; // No se encontró el final de la URL
  }

  // Extraer la URL
  std::string url = data.substr(start, end - start);

  return url;
}

std::string readFileContents(const std::string& filePath) {
  std::ifstream file(filePath.c_str());
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

bool Req::_run_location(std::string name, std::string httpRequest){

	size_t posicionEspacio = httpRequest.find(' ');
	std::string method_http;

  if (posicionEspacio != std::string::npos) {
      method_http = httpRequest.substr(0, posicionEspacio);
  } else {
      return false;
  }

	for (size_t i = 0; i < _server.get_location_size(); i++) {
		if (_server.get_location(i).get_name() == name) {

			bool find_method_http = false;
			for (size_t ii = 0; ii < _server.get_location(i).get_methods_size(); ii++)
			{
				if (method_http == _server.get_location(i).get_methods(ii))
					find_method_http = true;
			}
			if (!find_method_http)
				return false;

			std::string filePath = _server.get_location(i).get_root() + "/" + _server.get_location(i).get_index();
			env["FILE_NAME"] = filePath;
			env["SERVER_PROTOCOL"] = "HTTP/1.1";

			std::ifstream file(filePath.c_str());
    	if (!file.is_open()) {
    	  return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nFile not found.";
			}

			std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			file.close();

			_data_file = content;
			return true;
    }
  }
  return false;
}
