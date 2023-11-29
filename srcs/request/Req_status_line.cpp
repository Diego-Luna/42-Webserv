#include "Req.hpp"

Req::meth			Req::find_methode(std::string &methode)
{
	if (methode == "GET")
		return (&Req::getFonc);
	if (methode == "POST")
		return (&Req::postFonc);
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

u_int16_t Req::getFonc(std::string &element)
{

					// need to change to use fd
	// element = _location.get_root() + element;
	// if (element == _location.get_root() + "/")
	// 	element += "index.html"; // peut être remplacer par le root index
	// this->file.open(element);
	// this->file_name = element;
	// if (!file.is_open())
	// {
	// 	this->file_name = _location.get_root() + "/ErrorPages/404notFound.html"; // même chose ici
	// 	return (NOT_FOUND);
	// }
	// return(OK);
		(void)element;
		return (OK);
}

u_int16_t Req::postFonc(std::string &element)
{
	(void)element;
	
	std::string tmp;
	std::vector<std::string> info;
	tmp = this->_http_Req.substr(_http_Req.rfind('\n'));

	info = split(tmp, '&');
	std::cout << "voici les infos obtenue : " << std::endl;
	for (size_t i = 0; i < info.size(); i++)
		std::cout << info[i] << std::endl;
	
	return (OK_NO_CONTENT);
}


u_int16_t Req::parsing_status_line(std::vector<std::string> status_line)
{
	if (status_line.size() != 3 || (status_line[2] != "HTTP/1.1\r" && status_line[2] != "HTTP/1.1" ) || (this->methode = find_methode(status_line[0])) == 0)
	{	
		this->file_name = _location.get_root() + "/ErrorPages/400badRequest.html";
		return (BAD_REQUEST);
	}
	return ((this->*methode)(status_line[1]));
}

void Req::status_line_creation(const std::string &line)
{
	this->status_code = parsing_status_line(split(line, ' '));
	this->status_line = "HTTP/1.1 " + std::to_string(this->status_code)	 + " " + message_status_code(this->status_code) + "\r\n";
}
