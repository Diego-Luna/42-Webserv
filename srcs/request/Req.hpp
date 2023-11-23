#ifndef REQ_HPP
#define REQ_HPP

#include "../../Includes/Utils.hpp"
#include "../socket/Client/Client.hpp"

using std::string;


class Req {
private:
	std::ifstream	_file;
	string			_fileName;
	// header
	string			_RawHeader;	// as read from socket
	string			_body;
	Location		&_location;
	u_int16_t		_statusCode;
	
	client			_client;

	std::map<string, string>			_env;
	std::map<string, string>::iterator	itr_env;
	
	// method;
	typedef u_int16_t 		(Req::*meth)(string &element);
	meth 					methode;
	meth					find_methode(std::string &methode);
	u_int16_t 				getFunc(std::string &element);
	u_int16_t 				postFunc(std::string &element);

	// std constructor not available
	Req();
	
public:
	// cannonical form requirements
	Req(const Req &original);
	Req &operator=(const Req &next);
	~Req();
	Req(string HTTP_Req, const int fd, Location &location);
	
	// 	// status line
	string					statusLine;
	void 					statusLineCreation(const std::string &line);
	u_int16_t 				parsingStatusLine(std::vector<std::string> statusLine);
	const std::string		message_status_code(u_int16_t code);
	
	// getters and setters
	
	// methods
	char	**CGIExecEnv;
};

Req::Req() {
}

Req::Req(const Req &original) {
}

Req &Req::operator=(const Req &next) {
}

Req::~Req() {

}

// class Req
// {
// private:
// 	std::string 			status_line;
// 	std::string 			header;
// 	std::string 			body;
// 	std::string				file_name;
// 	std::string				http_Req;
// 	std::ifstream 			file;
// 	u_int16_t				status_code;
// 	client					b;
// 	Location				&_location;
	
// 	// u_int16_t			methode;
// 	typedef u_int16_t 		(Req::*meth)(std::string &element);
// 	meth 					methode;

// 	// status line
// 	void 					statusLineCreation(const std::string &line);
// 	// pt a supp
// 	u_int16_t 				parsingStatusLine(std::vector<std::string> status_line);
// 	meth					find_methode(std::string &methode);
// 	const std::string		message_status_code(u_int16_t code);
// 	// header
// 	void 					header_creation(void);
// 	std::string 			cType( void );
// 	// body
// 	void 					body_creation(void);
// 	//fonc
// 	u_int16_t 				getFonc(std::string &element);
// 	u_int16_t 				postFonc(std::string &element);
	
// public:
// 	int						fds[2]; // CGI fds

// 	static					std::vector<std::pair<std::string, std::string> > mime;
// 	std::string				getHttpString(void);
// 	std::string				get_header()const;
// 	u_int16_t				get_status_code() const;
// 	void 					set_status_code(u_int16_t statusCode_);
// 	Req						(std::string HTTP_Req, const int fd, Location &location);

// 	static void 			innitMime(void);
// 	~Req					();
// };


#endif