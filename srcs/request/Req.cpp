#include "Req.hpp"

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
	{
		parseHeader();
	}


	if (_isUpload == true && _error == false)
	{
		parseUpload();
		if (_validBodySize(_server.get_body_size()) == false)
		{
			_error = true;
			set_status_code(REQUEST_TOO_BIG);
		}
		if (_error == false)
			createUploadFile();
		Response response(*this);
		return;
	}
	else if (_isCGI && _error == false)
	{
		CGI	Cgi(*this);
	} else {

		// Obtiene el índice del servidor asociado
    std::string index = _server.get_index();

    // Verifica si el índice termina en ".html"
    bool isHtmlFile = index.length() >= 5 && index.substr(index.length() - 5) == ".html";

	std::vector<std::string> data = split(httpRequest, ' ');

	for (size_t l_size= 0; l_size < _server.get_location_size(); l_size++)
	{
		Location location_check = _server.get_location(l_size);
		std::string location_index = location_check.get_index();
		bool isHtmlFileLocation = location_index.length() >= 5 && location_index.substr(location_index.length() - 5) == ".html";
		if (!isHtmlFileLocation && data[1] == location_check.get_name())
		{
			std::string dirPath = location_check.get_root() + location_index;
			std:: string listDirectory = listDirectoryContents(dirPath.c_str());
			if (listDirectory != "Error")
			{
				status_code = 200;
				env["CONTENT_TYPE"] =  "text/html";
				env["FILE_NAME"] = "config-root-list-directory-server-dluna-lo's-and-gmiyakaw-team";
				Response response(*this, listDirectory);
			}
			return;
		}
	}

    if (!isHtmlFile && data[1] == "/") {
        // El índice no es un archivo .html, lista el contenido del directorio
        // Asume que el índice es el nombre del directorio dentro de la raíz del servidor
        std::string dirPath = _server.get_root() + "/" + index;
		std:: string listDirectory = listDirectoryContents(dirPath.c_str());
		if (listDirectory != "Error")
		{
			status_code = 200;
			env["FILE_NAME"] = "config-root-list-directory-server-dluna-lo's-and-gmiyakaw-team";
			Response response(*this, listDirectory);
		}
	}
    else {

		if (!isHtmlFile)
		{
			env["FILE_NAME"] = _server.get_root() + _server.get_index() + split(httpRequest, ' ')[1];
			if (f_check_path_line(env["FILE_NAME"], NULL)){
				status_code = 200;
			}
		}

        // Maneja la lógica existente
        if (_run_location(_extractURL(httpRequest), httpRequest) == false){
            Response response(*this);
        } else {
            status_code = 200;
            Response response(*this, this->_data_file);
        }
    }
	}
}

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
}

string	Req::getContentType(string &extension)
{
	if (extension.empty())
		return "";
	for (std::vector<std::pair<string, string> >::iterator it = mime.begin(); it != mime.end(); it++)
	{
		if (extension == it->first)
			return it->second;
	}
	// if extesion exists and it's not on MIME, 404 gets overuled by 415
	set_status_code(UNSUPPORTED_MEDIA);
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
	envCGIExecve[mapSize] = NULL;

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
	// if (_pathInfo != "")
	// {
	// 	if (access(_pathInfo.c_str(), F_OK) != 0)
	// 	{
	// 		perror("Path Info file not found");
	// 		set_status_code(BAD_REQUEST);
	// 		_error = true;
	// 		return;
	// 	}
	// }
	if (_validPort(env["HOST"]) == false)
	{
		set_status_code(BAD_REQUEST);
		_error = true;
	}
	if (_validBodySize(_server.get_body_size()) == false)
	{
		set_status_code(REQUEST_TOO_BIG);
		_error = true;
		std::cerr << "BODY TOO BIG ERROR" << endl;
	}

	set_status_code(OK);

}

bool	Req::_validBodySize(int bodySize) {
	if (_server.get_body_size() == -1) // means it never got set in config, therefore any value is good
		return true;
	if ( std::atoi(env["CONTENT_LENGTH"].c_str()) > bodySize * 1000000) // assumes .config is dealing in megabytes
		return false;
	return true;
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
			for (size_t ii = 0; ii < _server.get_location(i).get_methods_size() && find_method_http == false; ii++)
			{
				if (method_http == _server.get_location(i).get_methods(ii))
					find_method_http = true;
			}
			if (!find_method_http)
				return false;

			std::string filePath = _server.get_location(i).get_root() + "/" + _server.get_location(i).get_index();
			env["FILE_NAME"] = filePath;
			env["SERVER_PROTOCOL"] = "HTTP/1.1";

			// std::ifstream file(filePath.c_str());
    		// if (!file.is_open()) {
    	  		// return "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nFile not found.";
			// }

			// std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
			// file.close();

			// _data_file = content;
			return true;
    }
  }
  return false;
}

/**************************************************************************
		Server
**************************************************************************/

std::string	Req::return_map_server_pages(std::string index){
	return _server.get_error_page(index);
}


/**************************************************************************
		Server folder
**************************************************************************/
std::string Req::listDirectoryContents(const char *dirPath) {
    DIR *dir;
    struct dirent *ent;
    // char message[BUFFER_SIZE] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Directory Listing</h1><ul>";
    char message[BUFFER_SIZE] = "";

    if ((dir = opendir(dirPath)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') { // Ignore hidden files (and parent/current directories)
                strcat(message, "<li><a href=\"");
                strcat(message, ent->d_name);
                strcat(message, "\">");
                strcat(message, ent->d_name);
                strcat(message, "</a></li>");
            }
        }
        closedir(dir);
    } else {
        // Could not open directory
        perror("Could not open directory");
        return "Error";
    }

    strcat(message, "</ul></body></html>");
		return message;
}