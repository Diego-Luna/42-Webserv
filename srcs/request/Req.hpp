#ifndef REQ_HPP
#define REQ_HPP

#include "../../Includes/Utils.hpp"

#include "../socket/Client/Client.hpp"
#include <sstream>
#include <cstring>

using std::cout;
using std::endl;
using std::string;

class Req
{
private:
	Req();

	Client					_client;
	Location				&_location;
	// Server					&_server;
	string	 				_header;
	string 					_body;
	string					_method;
	string					_http_Req;
	string					_fileName;
	string					_pathInfo;
	string					_querryString;
	std::istringstream		_ReqStream;
	bool					_isCGI;

	// header parsing
	void					parseHeader(void);
	void					parseFirstLine(void);
	bool					_validMethod(const string &line);
	bool					_validPath(string &line);
	bool					_validVersion(string &line);
	bool					_checkCGI(string &firstLine);
	size_t					_findExtensionEnd(string &line);
	string					_getQuerryString(string &line);
	
	// CGI AND ENV PREP	
	string					_formatStringEnvCGI(string str);
	void					_populateEnvCGI(string var);
	void					_makeEnvCGI(void);
	void					_makeExecveEnv(void);
	bool					_isValidVariable(string &var);
	string					_decodeURI(string	str);
	void					_buildEncoded();
	std::map<string, string>	_encoded;
	
	// VALIDATION
	void					_validate();
	bool					_allValidCharsURI(string str);
	bool					_isValidCharURI(uint8_t ch);

/*
	below this line: things that were here before but I dont understand
	the functionallity of yet.
	
	Some of these seem to be better suited for Response, rather than Request
*/
	void 					header_creation(void);	// unable to test, more details inside
	std::string 			cType( void );
	std::string 			status_line;
	u_int16_t				status_code;
	void 					body_creation(void);
	// body
	//fonc
	u_int16_t 				getFonc(std::string &element);
	u_int16_t 				postFonc(std::string &element);
	
	// u_int16_t			methode;
	typedef u_int16_t 		(Req::*meth)(std::string &element);
	meth 					methode;
	// status line /
	void 					status_line_creation(const std::string &line);
	// pt a supp
	u_int16_t 				parsing_status_line(std::vector<std::string> status_line);
	meth					find_methode(std::string &methode);
	const std::string		message_status_code(u_int16_t code);

public:
	Req						(std::string HTTP_Req, const int fd, Location &location);

	std::map<string, string>	env;
	char						**envCGIExecve;		// must be deleted by destructor
	int							fds[2];

		// GETTER / SETTER
	string					getHttpString()const;
	string					get_header()const;
	u_int16_t				get_status_code() const;

	void					printReq();

	void 					set_status_code(u_int16_t statusCode_);
			// what is mime?
	/*
		Multipurpose Internet Mail Extension
		which files extensions are accepted by the webserver.
				- why is this in the Request?
				- should probably be in the Server, which should be accessible by the Request
				CONTENT_TYPE?
	*/
	static					std::vector<std::pair<std::string, std::string> > mime;
	static void 			innitMime(void);
	~Req					();
};


#endif