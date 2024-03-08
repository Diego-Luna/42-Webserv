#include "CGI.hpp"

#define PIPE_READ 1
#define PIPE_WRITE 0
// Constructor implementation
CGI::CGI(Req &req_) : req(req_), m_server() {
	string responseBody = exec();

	Response response(req, responseBody);
		// MAKE REPONSE CLASS AND SEND BODY, EVEN IF EMPTY
}

CGI::~CGI() {}

string CGI::exec() {

	// std::cout << "-> Diego -> req.envCGIExecve:{";
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
    ssize_t bytesRead;

    lseek(fdOut, 0, SEEK_SET);
    while ((bytesRead = read(fdOut, buffer, BUFFER_SIZE)) > 0) {
        responseBody.append(buffer, bytesRead);
    }

	close(fdOut);
    req.env["CONTENT_LENGTH"] = size_tToString(responseBody.length());
	return responseBody;
}
