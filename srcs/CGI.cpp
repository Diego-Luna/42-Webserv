#include "CGI.hpp"

#define PIPE_READ 1
#define PIPE_WRITE 0
// Constructor implementation
CGI::CGI(Req &req_) : req(req_), m_server() {
	string responseBody = exec();

					cout << "printing response body\n" << responseBody << endl;
	Response response(req, responseBody);
		// MAKE REPONSE CLASS AND SEND BODY, EVEN IF EMPTY
}

CGI::~CGI() {}

string CGI::exec() {

	std::cout << "-> Diego -> req.envCGIExecve:{";


	size_t envCount = 0;
	while (req.envCGIExecve[envCount])
	{
		std::cout << req.envCGIExecve[envCount] << "\n";
		envCount++;
	}

	char** newEnv = new char*[envCount + 2]; // +2 for DATA_CGI and NULL terminator

	// Copy existing variables
  for (size_t i = 0; i < envCount; ++i) {
      newEnv[i] = new char[strlen(req.envCGIExecve[i]) + 1];
      strcpy(newEnv[i], req.envCGIExecve[i]);
  }

	// Prepare DATA_CGI variable
  string dataCGIValue = "DATA_CGI=" + req.getBody();
  newEnv[envCount] = new char[dataCGIValue.length() + 1];
  strcpy(newEnv[envCount], dataCGIValue.c_str());

  // Null terminate the new environment array
  newEnv[envCount + 1] = NULL;


	std::cout << "}" << std::endl;

	std::cout << "-> Diego -> req.getBody():{" << req.getBody() << "}" << std::endl;

    int pfd[2], input_pfd[2];
    if (pipe(pfd) == -1 || pipe(input_pfd) == -1) {
        perror("pipe failed");
        return "";
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        close(pfd[0]);
        close(pfd[1]);
        close(input_pfd[0]);
        close(input_pfd[1]);
        return "";
    }

    if (pid == 0) { // Child process
        dup2(pfd[1], STDOUT_FILENO);
        dup2(input_pfd[0], STDIN_FILENO);
        close(pfd[0]); close(pfd[1]);
        close(input_pfd[0]); close(input_pfd[1]);

				const char* argv[] = { "python3", req.env["FILE_NAME"].c_str(), NULL };
				execve("/usr/bin/python3", const_cast<char* const*>(argv), newEnv);

        perror("execve failed");
        exit(EXIT_FAILURE);
    } else { // Parent process
        close(pfd[1]);
        close(input_pfd[0]);
        close(input_pfd[1]);

        // Handling the termination of the child process
        int status;
        waitpid(pid, &status, 0); // Simplify this with waitpid()

				// In the parent process,
    		for (size_t i = 0; i < envCount + 1; ++i) { // +1 to include DATA_CGI
    		    delete[] newEnv[i]; // Free the individual strings
    		}
    		delete[] newEnv;

        // Now, instead of manually reading output and forming the response,
        // let's use the makeBody function to do that.
        string body = makeBody(pfd[0]);
        close(pfd[0]);

        // Check if the child process exited normally
        if (WIFEXITED(status)) {
            // Response can be constructed with Req and body,
            // and automatically sets the right headers and status.
            Response response(req, body); // Adjust according to your Response class design
            // responseString is a member of Req or wherever you need it
            // req.responseString = response->constructResponse(); // This method should return the full HTTP response
            return body; // Or return the full response as needed
        } else {
            req.set_status_code(INTERNAL_SERVER_ERROR);
            return "";
        }
    }
}

string	CGI::makeBody(int fdOut)
{
	req.set_status_code(OK);
	string	responseBody;
	char buffer[BUFFER_SIZE];
	int	finishedReading = 1;

			// poll() setup
	struct pollfd pollContext;
	pollContext.fd = fdOut;
	pollContext.events = POLLIN;
	int	pollReturn = poll(&pollContext, 1, -1);
	if (pollReturn == -1) {
		req.set_status_code(INTERNAL_SERVER_ERROR);
		close(fdOut);
		return "";
	}

	lseek(fdOut, 0, SEEK_SET);
	while (finishedReading > 0)
	{
		if (pollContext.revents & POLLIN)
			finishedReading = read(fdOut, buffer, BUFFER_SIZE);
		if (finishedReading > 0)
			responseBody.append(buffer, finishedReading);
	}
	close(fdOut);
	req.env["CONTENT_LENGTH"] = std::to_string(responseBody.length());
	return responseBody;
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



/******************
 * ORIGINAL VERSION BELOW
*******************/



// #define PIPE_READ 1
// #define PIPE_WRITE 0
// // Constructor implementation
// CGI::CGI(Req &req_) : _req(req_),  m_server() {
// }

// CGI::~CGI() {}

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
//     if (_req.getHttpString() == "GET")
// 		return "GET";
// 	else if (_req.getHttpString() == "POST")
// 		return "POST";
// 	else if (_req.getHttpString() == "DELETE")
// 		return "DELETE";
//     return "UNKNOWN";
// }




// // Member function to get the path info
// std::string CGI::m_pathInfoGet() {
//     // Implement the m_pathInfoGet() function
//     return "";
// }







// // Member function to get the query string
// std::string CGI::m_queryStringGet() {
//     // Implement the m_queryStringGet() function
//     return "";
// }

// std::string CGI::m_headerGet(std::string header_) {
//     if (_req.get_header() == header_ ) {
//         return _req.get_header();
//     }
//     return "";
// }




// // Member function to execute CGI
// void CGI::exec() {
 

// 	char *path = strdup("data/cgi/CGI.py");
// 	char *args[] = {path, NULL};

// 	pid_t pid = fork();


// 	if (pid == -1) {
//         std::cout<< "Error while forking process." << std::endl;
//         _req.set_status_code(500);
// 	}
// 	else if (pid == 0) {
//         std::cout << "Executing CGI script..." << std::endl;

// 		dup2(_req.fds[PIPE_WRITE], 1);
// 		close(_req.fds[PIPE_READ]);

// 		char **_envp = vecToChar(_env);

// 		execve(args[0], args, _envp);
//         std::cout << "Error while trying to execute CGI script." << std::endl;
// 		close(_req.fds[PIPE_WRITE]);
// 		_req.set_status_code(500);
// 		exit(1);
// 	}
// }
//     // Implement the executeCgi() function
 