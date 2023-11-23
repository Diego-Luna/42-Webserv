#include "Req.hpp"

std::string Req::cType( void )
{
	std::string tmp;

	size_t pos = this->file_name.find('.');
	if (pos == std::string::npos)
		return(std::string("application/octet-stream"));
	tmp = this->file_name.substr(pos);
	for (size_t i = 0; i < mime.size(); i++)
	{
		if (tmp == mime[i].first)
			return (mime[i].second);
	}
	return (std::string("application/octet-stream"));
}

void Req::header_creation(void)
{
	this->header = "Host: " + std::string(inet_ntoa(b.getAddr().sin_addr)) + ":" + std::to_string(ntohs(b.getAddr().sin_port)) + "\r\n";
	if (this->methode == &Req::getFunc || this->_statusCode == 404 || this->_statusCode == 400)
	{
		this->header += "Content-Type: " + cType() + "; charset=UTF-8\r\n"; // peut PEUT ËTRE causé un problme (utf-8 etc.)
		this->header += "Content-Length: " + std::to_string(this->_body.length()) + "\r\n";
	}

	// 				User-Agent
	this->header += "\r\n";
}