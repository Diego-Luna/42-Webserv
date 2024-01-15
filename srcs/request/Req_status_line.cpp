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
		this->_fileName = _location.get_root() + "/ErrorPages/400badRequest.html";
		return (BAD_REQUEST);
	}
	return ((this->*methode)(status_line[1]));
}

// void Req::status_line_creation(const std::string &line)
// {
// 	this->status_code = parsing_status_line(split(line, ' '));
// 	this->status_line = "HTTP/1.1 " + std::to_string(this->status_code)	 + " " + message_status_code(this->status_code) + "\r\n";
// }
