#ifndef REQ_HPP
#define REQ_HPP

#include "../../Includes/Utils.hpp"

#include "../socket/Client/Client.hpp"
#include <sstream>

using std::cout;
using std::endl;
using std::string;


class Req
{
private:
	std::string 			status_line;
	std::string 			_header;
	std::string 			_body;

	std::string				file_name;		// which file?? html?

	u_int16_t				status_code;
	client					_client;
	Location				&_location;
	string					http_Req;
	std::istringstream		_ReqStream;
	bool					_isCGI;
	string					_method;
	
	// u_int16_t			methode;
	typedef u_int16_t 		(Req::*meth)(std::string &element);
	meth 					methode;




	// status line
	void 					status_line_creation(const std::string &line);
	// pt a supp
	u_int16_t 				parsing_status_line(std::vector<std::string> status_line);
	meth					find_methode(std::string &methode);
	const std::string		message_status_code(u_int16_t code);
	// header
	void					parseHeader(void);
	void					parseFirstLine(void);
	bool					_hasMultipleMethods(const string &line);
	void 					header_creation(void);	// unable to test, more details inside
	std::string 			cType( void );
	// body
	void 					body_creation(void);
	//fonc
	u_int16_t 				getFonc(std::string &element);
	u_int16_t 				postFonc(std::string &element);
	
	
public:
	int						fds[2]; // CGI fds

	static					std::vector<std::pair<std::string, std::string> > mime;
	std::string				getHttpString(void);
	std::string				get_header()const;
	u_int16_t				get_status_code() const;
	void 					set_status_code(u_int16_t statusCode_);
	Req						(std::string HTTP_Req, const int fd, Location &location);


	void					printReq();
	static void 			innitMime(void);
	~Req					();
};


#endif