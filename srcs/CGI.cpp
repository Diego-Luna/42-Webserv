#include "CGI.hpp"

#define PIPE_READ 1
#define PIPE_WRITE 0
// Constructor implementation
CGI::CGI(Req &req_) : _req(req_),  m_server() {
}

CGI::~CGI() {}

void CGI::m_setEnv() {
	_env["SERVER_SOFTWARE"] = "webserv/1.0";
	if (m_server.get_name().size() > 0)
		_env["SERVER_NAME"] = m_server.get_name();
	else
		_env["SERVER_NAME"] = "webserv";
	_env["GATEWAY_INTERFACE"] = "CGI/1.1";
	_env["SERVER_PROTOCOL"] = "HTTP/1.1";
	
								// unchecked or missing below this line
	
	_env["REQUEST_METHOD"] = m_httpRequestGet();
	_env["PATH_INFO"] = m_pathInfoGet();
	_env["SCRIPT_NAME"] = m_scriptGet();
	_env["QUERY_STRING"] = m_queryStringGet();
	
	// _env["SERVER_PORT"] = std::to_string(m_server.get_ports()([0]));		// NEEDS CLARIFICATION
	
	_env["CONTENT_TYPE"] = m_headerGet("CONTENT-TYPE");				// CHECK FUNCTION
	_env["CONTENT_LENGTH"] = m_headerGet("CONTENT-LENGTH");
	_env["HTTP_ACCEPT"] = m_headerGet("ACCEPT");
	_env["HTTP_USER_AGENT"] = m_headerGet("USER-AGENT");
	_env["HTTP_COOKIE"] = m_headerGet("COOKIE");
	
			// OLD, deprecated and being replaced
    // _env.push_back("SERVER_SOFTWARE=webserv/1.0");
    // if (m_server.get_name().size() > 0)
    //     _env.push_back("SERVER_NAME=" + m_server.get_name());
    // else
    //     _env.push_back("SERVER_NAME=webserv");
    // _env.push_back("GATEWAY_INTERFACE=CGI/1.1");

    // _env.push_back("SERVER_PROTOCOL=HTTP/1.1");
    // _env.push_back("SERVER_PORT=" + std::to_string(m_server.get_ports()[0]));
    // _env.push_back("REQUEST_METHOD=" + m_httpRequestGet());
    // _env.push_back("PATH_INFO=" + m_pathInfoGet());
    // _env.push_back("SCRIPT_NAME=" + m_scriptGet());
    // _env.push_back("QUERY_STRING=" + m_queryStringGet());

    // _env.push_back("CONTENT_TYPE=" + m_headerGet("CONTENT-TYPE"));
    // _env.push_back("CONTENT_LENGTH=" + m_headerGet("CONTENT-LENGTH"));

    // _env.push_back("HTTP_ACCEPT=" + m_headerGet("ACCEPT"));
    // _env.push_back("HTTP_USER_AGENT=" + m_headerGet("USER-AGENT"));
    // _env.push_back("HTTP_COOKIE=" + m_headerGet("COOKIE"));
}


// Member function to get the script
std::string CGI::m_scriptGet() {
	
	
	
	
	
    return "";
}

// Member function to get the HTTP request
std::string CGI::m_httpRequestGet() {
    if (_req.getHttpString() == "GET")
		return "GET";
	else if (_req.getHttpString() == "POST")
		return "POST";
	else if (_req.getHttpString() == "DELETE")
		return "DELETE";
    return "UNKNOWN";
}




// Member function to get the path info
std::string CGI::m_pathInfoGet() {
	return "";
}







// Member function to get the query string
std::string CGI::m_queryStringGet() {
    // Implement the m_queryStringGet() function
    return "";
}

std::string CGI::m_headerGet(std::string header_) {
    if (_req.get_header() == header_ ) {
        return _req.get_header();
    }
    return "";
}




// Member function to execute CGI
void CGI::exec() {
 

	// char *path = strdup("data/cgi/CGI.py");
	// char *args[] = {path, NULL};

	// pid_t pid = fork();


	// if (pid == -1) {
    //     std::cout<< "Error while forking process." << std::endl;
    //     _req.set_status_code(500);
	// }
	// else if (pid == 0) {
    //     std::cout << "Executing CGI script..." << std::endl;

	// 	dup2(_req.fds[PIPE_WRITE], 1);
	// 	close(_req.fds[PIPE_READ]);


	// 	char **_envp = vecToChar(_env);			// might need update

	// 	execve(args[0], args, _envp);
    //     std::cout << "Error while trying to execute CGI script." << std::endl;
	// 	close(_req.fds[PIPE_WRITE]);
	// 	_req.set_status_code(500);
	// 	exit(1);
	// }
}
    // Implement the executeCgi() function
 