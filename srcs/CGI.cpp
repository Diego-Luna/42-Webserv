#include "CGI.hpp"

#define PIPE_READ 1
#define PIPE_WRITE 0
// Constructor implementation
CGI::CGI(Req &req_) : req(req_), m_server() {
	string responseBody = exec();

					cout << "printing response body\n" << responseBody << endl;
				// MAKE REPONSE CLASS AND SEND BODY, EVEN IF EMPTY
}

CGI::~CGI() {}

string CGI::exec() {
		// which script to run
	char *path =  strdup(req.env["PATH_INFO"].c_str());			// must be freed
	char *args[] = {path, NULL};

	FILE	*scriptOut = tmpfile();

	long	fdOut = fileno(scriptOut);

	pid_t pid = fork();
	if (pid == -1) {
        std::cout<< "Error while forking process." << std::endl;
		if (path)
		{
			free(path);
			path = NULL;
		}
		req.set_status_code(500);
	} else if (pid == 0) {	// child process
							std::cout << "Executing CGI script..." << std::endl;
							// cout << "path: " << path << endl;
							// cout << "arg[0]: " << args[0] << endl;

		dup2(fdOut, STDOUT_FILENO);

		execve(args[0], args, req.envCGIExecve);
		std::cerr << "Error while trying to execute CGI script." << std::endl;
				// fclose(scriptOutput);
		req.set_status_code(500);
		if (path)
		{
			free(path);
			path = NULL;
		}
	}
	int	status;
	pid_t	terminatedProcess = 0;
	if (path)
	{
		free(path);
		path = NULL;
	}
	do {
		terminatedProcess = waitpid(terminatedProcess, &status, 0);
		if (terminatedProcess == -1) // error in the process
		{
			req.set_status_code(500);
			return "";
		}
		if (WIFEXITED(status))
		{
			req.set_status_code(200);	// process exited normally
			string	responseBody;
			char buffer[BUFFER_SIZE];
			int	finishedReading = 1;
			
			lseek(fdOut, 0, SEEK_SET);
			
			while (finishedReading > 0)
			{
				finishedReading = read(fdOut, buffer, BUFFER_SIZE);
				if (finishedReading > 0)
					responseBody.append(buffer, finishedReading);
			}

			close(fdOut);
			return responseBody;
		}
		else if (WIFSIGNALED(status))
		{
			req.set_status_code(500);	// process interrupted by signal
			return "";
		}
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	return "";
}


		// deprecated. Might have some useful ideas for environment variables that
		// haven't been added just yet
// void CGI::m_setEnv() {
//     _env.push_back("SERVER_SOFTWARE=webserv/1.0");
//     if (m_server.get_name().size() > 0)
//         _env.push_back("SERVER_NAME=" + m_server.get_name());
//     else
//         _env.push_back("SERVER_NAME=webserv");
//     _env.push_back("GATEWAY_INTERFACE=CGI/1.1");

//     _env.push_back("SERVER_PROTOCOL=HTTP/1.1");
//     _env.push_back("SERVER_PORT=" + std::to_string(m_server.get_ports()[0]));
//     _env.push_back("REQUEST_METHOD=" + m_httpRequestGet());
//     _env.push_back("PATH_INFO=" + m_pathInfoGet());
//     _env.push_back("SCRIPT_NAME=" + m_scriptGet());
//     _env.push_back("QUERY_STRING=" + m_queryStringGet());

//     _env.push_back("CONTENT_TYPE=" + m_headerGet("CONTENT-TYPE"));
//     _env.push_back("CONTENT_LENGTH=" + m_headerGet("CONTENT-LENGTH"));

//     _env.push_back("HTTP_ACCEPT=" + m_headerGet("ACCEPT"));
//     _env.push_back("HTTP_USER_AGENT=" + m_headerGet("USER-AGENT"));
//     _env.push_back("HTTP_COOKIE=" + m_headerGet("COOKIE"));
// }




// // Member function to get the script
// std::string CGI::m_scriptGet() {
//     // Implement the m_scriptGet() function
//     return "";
// }

// // Member function to get the HTTP request
// std::string CGI::m_httpRequestGet() {
//     if (req.getHttpString() == "GET")
// 		return "GET";
// 	else if (req.getHttpString() == "POST")
// 		return "POST";
// 	else if (req.getHttpString() == "DELETE")
// 		return "DELETE";
//     return "UNKNOWN";
// }

// // Member function to get the path info
// std::string CGI::m_pathInfoGet() {
// 	if (!std::getenv("PATH_INFO"))
// 		throw std::invalid_argument("PATH_INFO not set");		// uncaught so far


// 	return std::getenv("PATH_INFO");
// }

// // Member function to get the query string
// std::string CGI::m_queryStringGet() {
    
//     return "";
// }

// std::string CGI::m_headerGet(std::string header_) {
//     if (req.get_header() == header_ ) {
//         return req.get_header();
//     }
//     return "";
// }
