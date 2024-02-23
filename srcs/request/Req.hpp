#ifndef REQ_HPP
#define REQ_HPP

#include "../../Includes/Utils.hpp"
#include "../socket/Client/Client.hpp"
#include "../CGI.hpp"
#include "../response/Response.hpp" // Asegúrate de usar la ruta correcta al archivo de cabecera


#include <sstream>
#include <iostream>
#include <cstring>
#include <string>

#define PATH_TO_INDEX "data/www/Pages/index.html"
#define PATH_TO_ROOT "data/www/Pages"
#define PATH_TO_CGI "CGI/bin"
#define PATH_TO_UPLOAD "data/www/upload/"
#define POLL_INF_TIME -1

using std::cout;
using std::endl;
using std::string;

class CGI;
class Response;

class Req
{
private:
	Req();
	Req(const Req &original);

	Location				&_location;
	Server					_server;
	string					_root;
	string	 				_header;
	string 					_body;
	string					_method;
	string					_http_Req;
	string					_fileName;
	string					_pathInfo;
	string					_protocol;
	string					_extension;
	string					_querryString;
	string					_data_file; // locations
	std::istringstream		_ReqStream;
	bool					_isCGI;
	bool					_error;
	u_int16_t				status_code;

	// HEADER PARSING
	void					parseHeader(void);
	void					parseFirstLine(void);
	bool					_validMethod(const string &line);
	bool					_validPath(string &line);
	bool					_validVersion(string &line);
	bool					_validPort(string host);
	bool					_checkCGI(string &fileName);
	size_t					_findExtensionEnd(string &line);
	string					_getQuerryString(string &line);
	string					getContentType(string &extension);
	
	// UPLOAD MANAGEMENT
	void					parseUpload(void);
	string					findUploadFileName(string boundary);
	string					findUploadBoundry(void);
	void					stripQuotes(string &original);
	string					findUploadBody(string boundary);
	string					trimLine(string line);
	void					createUploadFile();

	// CGI AND ENV PREP	
	string					_formatStringEnvCGI(string str);
	void					_populateEnv(string var);
	void					_makeEnv(void);
	void					_makeExecveEnv(void);
	bool					_isValidVariable(string &var);
	string					_decodeURI(string	str);
	void					_buildEncoded();
	std::map<string, string>	_encoded;
	
	// VALIDATION
	void					_validate();
	bool					_allValidCharsURI(string str);
	bool					_isValidCharURI(uint8_t ch);

	// Diego - location
	// std::string _extractURL(const std::vector<char>& dataVector);
	std::string _extractURL(std::string &dataVector);
	bool _run_location(std::string name, Location &location);

public:
	Req(Server _server, string httpRequest, const int fd, Location &location, listenner &listenner_);
	~Req();
	client						_client;
	listenner					&_listenner;
	std::map<string, string>	env;
	string						responseString;
	char						**envCGIExecve;		// must be deleted by destructor
	static						std::vector<std::pair<std::string, std::string> > mime;
	static void 				innitMime(void);

		// GETTER / SETTER
	string					getHttpString()const;
	string					get_header()const;
	string					getRoot()const;
	string					getBody()const;
	bool					getIsCGI() const;
	bool					_isUpload;
	u_int16_t				get_status_code() const;
	void 					set_status_code(u_int16_t statusCode_);
	void					printReq();
};

#endif