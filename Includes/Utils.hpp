#ifndef UTILS_HPP
#define UTILS_HPP

#include "Parsing.hpp"

#ifndef EXCEPTION_MESSAGES_HPP
# define EXCEPTION_MESSAGES_HPP

//Exception messages Request
# define ERR_REQUEST_SIZE "Request size is too big"
# define ERR_SOCKET_READ "Error reading socket"

# define ERR_NO_CRLF "Invalid line: no CRLF"
# define ERR_INVALID_LINE "Invalid line: Invalid line"

# define ERR_START_LINE_INVALID_METHOD "Invalid start line: invalid method"
# define ERR_START_LINE_INVALID_VERSION "Invalid start line: invalid version"
# define ERR_START_LINE_NOT_ALLOWED_METHOD "Invalid start line: not allowed method"
# define ERR_START_LINE_INVALID_URI "Invalid start line: invalid URI"

# define ERR_HEADER_DUPLICATE "Invalid header: duplicate header"
# define ERR_HEADER_MISSING_HEADER "Invalid header: missing header"
# define ERR_HEADER_INVALID_VALUE "Invalid header: invalid value"
# define ERR_HEADER_INVALID_KEY "Invalid header: invalid key"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <iostream>
#include <poll.h>
#include <fstream>
#include <vector>
#include <utility>
#include <sstream>


#endif


#define DEFAULT_PORT 8080	

# define MESSAGE_OK 						"OK"
# define MESSAGE_OK_CREATED 				"Created"
# define MESSAGE_OK_NO_CONTENT				"No Content"
# define MESSAGE_MOVED_TO_NEW_URL 			"Moved to New Url"
# define MESSAGE_NOT_MODIFIED 				"Not Modified"
# define MESSAGE_BAD_REQUEST 				"Bad Request"
# define MESSAGE_UNAUTHORIZED 				"Unauthorized"
# define MESSAGE_NOT_FOUND 					"Not Found"
# define MESSAGE_INTERNAL_SERVER_ERROR		"Internal Server Error"
# define MESSAGE_VERS_NOT_SUPPORTED			"HTTP Version Not Supported"
# define MESSAGE_UNSUPPORTED_MEDIA			"Unsupported Media Type"

enum		status_code
{
	OK = 200,
	OK_CREATED = 201,
	OK_NO_CONTENT = 204,
	MOVED_TO_NEW_URL = 301,
	NOT_MODIFIED = 304,
	BAD_REQUEST = 400,
	UNAUTHORIZED = 401,
	UNSUPPORTED_MEDIA = 415,
	NOT_FOUND = 404,
	INTERNAL_SERVER_ERROR = 500,
	HTTP_VERSION_NOT_SUPPORTED = 505
};

enum		HTTP_METHODE
{
	GET = 1,
	POST,
	PUT,
	DELETE
};

std::vector<std::string> split(const std::string &input, char c);
void fatal(const char *msg);
char **vecToChar(std::vector<std::string> &vec);

/*  PROTOTYPES    */
std::string size_tToString(size_t number);

#endif