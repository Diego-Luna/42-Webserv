#include "Req.hpp"

Req::meth			Req::find_methode(std::string &methode)
{
	if (methode == "GET")
		return (&Req::getFunc);
	if (methode == "POST")
		return (&Req::postFunc);
	if (methode == "PUT")
		return (0);
	if (methode == "DELETE")
		return (0);
	return (0);
}

const std::string Req::message_status_code(u_int16_t code)
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

u_int16_t Req::getFunc(std::string &element)
{
	element = _location.get_root() + element;
	if (element == _location.get_root() + "/")
		element += "index.html"; // peut être remplacer par le root index
	this->_file.open(element);
	this->_fileName = element;
	if (!_file.is_open())
	{
		this->_fileName = _location.get_root() + "/ErrorPages/404notFound.html"; // même chose ici
		return (NOT_FOUND);
	}
	return(OK);
}

u_int16_t Req::postFunc(std::string &element)
{
	(void)element;
	
	std::string tmp;
	std::vector<std::string> info;
	tmp = this->http_Req.substr(http_Req.rfind('\n'));

	info = split(tmp, '&');
	std::cout << "voici les infos obtenue : " << std::endl;
	for (size_t i = 0; i < info.size(); i++)
		std::cout << info[i] << std::endl;
	
	return (OK_NO_CONTENT);
}


u_int16_t Req::parsingStatusLine(std::vector<std::string> statusLine)
{
	if (statusLine.size() != 3 || (statusLine[2] != "HTTP/1.1\r" && statusLine[2] != "HTTP/1.1" ) || (this->methode = find_methode(statusLine[0])) == 0)
	{	
		this->_fileName = _location.get_root() + "/ErrorPages/400badRequest.html";
		return (BAD_REQUEST);
	}
	return ((this->*methode)(statusLine[1]));
}

void Req::statusLineCreation(const std::string &line)
{
	this->_statusCode = parsingStatusLine(split(line, ' '));
	this->statusLine = "HTTP/1.1 " + std::to_string(this->_statusCode)	 + " " + message_status_code(this->_statusCode) + "\r\n";
}
