#include "Req.hpp"

std::string		Req::getHttpString(void)
{
	return (std::string(this->status_line + this->header + this->body));
}

void Req::body_creation(void)
{
	if (!this->file.is_open())
	{
		if (this->status_code == 404)	
			this->file.open(this->file_name);
		else if (this->status_code == 400)
			this->file.open(this->file_name);
		if (!this->file.is_open())
			fatal("open");
	}
	this->body.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
	file.close();
}

Req::Req(std::string HTTP_Req, const int fd, Location &location) : b(fd), _location(location)
{

	if (HTTP_Req.length()  < 1)
		fatal("Bad HTTP REQUEST");
	this->http_Req = HTTP_Req;
	status_line_creation(HTTP_Req.substr(0, HTTP_Req.find('\n')));
	if (this->methode == &Req::getFonc || this->status_code == 404 || this->status_code == 400)
	{
		body_creation();
	}
	header_creation();
}


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
	return this->header;
}


Req::~Req()
{
}