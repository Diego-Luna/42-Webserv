#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio> // For fprintf
#include <cstring> // For strlen

const int PORT = 8080;

extern char **environ;  // Declaration of the environ variable

// Assuming you already have the rest of the includes and your executeCGI function starts here

void executeCGI(const std::string& scriptPath, int clientSocket, const std::string& method, const std::string& postData) {
    int pfd[2], input_pfd[2];
    if (pipe(pfd) == -1 || pipe(input_pfd) == -1) {
        perror("pipe failed");
        return;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork failed");
        close(pfd[0]);
        close(pfd[1]);
        close(input_pfd[0]);
        close(input_pfd[1]);
        return;
    }

    if (pid == 0) { // Child process
        close(pfd[0]); // Close the reading end of the output pipe
        close(input_pfd[1]); // Close the writing end of the input pipe

        dup2(pfd[1], STDOUT_FILENO); // Duplicate the writing end of the output pipe to stdout
        dup2(input_pfd[0], STDIN_FILENO); // Duplicate the reading end of the input pipe to stdin

        close(pfd[1]); // Close the duplicate
        close(input_pfd[0]); // Close the duplicate

        setenv("REQUEST_METHOD", method.c_str(), 1);
        if (method == "POST") {
            // Set CONTENT_LENGTH as an environment variable
            std::string contentLengthStr = std::to_string(postData.length());
            setenv("CONTENT_LENGTH", contentLengthStr.c_str(), 1);
        }

        const char* argv[] = { "python3", scriptPath.c_str(), NULL };
        execve("/usr/bin/python3", const_cast<char* const*>(argv), environ);

        perror("execve failed");
        exit(EXIT_FAILURE);
    } else { // Parent process
        close(pfd[1]); // Close the writing end of the output pipe
        close(input_pfd[0]); // Close the reading end of the input pipe

        // If the method is POST, write the POST data into the input pipe
        if (method == "POST") {
            write(input_pfd[1], postData.c_str(), postData.length());
        }

        close(input_pfd[1]); // Close the writing end after sending the data

        // Read the output from the CGI script from the output pipe and send it to the client
        char buffer[128];
        std::string result = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        ssize_t bytes_read;
        while ((bytes_read = read(pfd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';
            result += buffer;
        }
        close(pfd[0]);

        waitpid(pid, NULL, 0);

        write(clientSocket, result.c_str(), result.size());
    }
}

void serveForm(int clientSocket) {
    // Path to the HTML file you want to serve
    std::string filePath = "./form.html";

    // Open the file in reading mode
    std::ifstream fileStream(filePath, std::ios::binary);

    if(fileStream) {
        // Read the file's content
        std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        std::stringstream buffer;
        buffer << fileStream.rdbuf();

        // Add the file content to the HTTP response
        httpResponse += buffer.str();

        // Send the response to the client
        send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
    } else {
        // If the file is not found, send a 404 error response
        std::string errorResponse = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>";
        send(clientSocket, errorResponse.c_str(), errorResponse.length(), 0);
    }
}

void handleClient(int clientSocket) {
    char buffer[1024] = {0};
    read(clientSocket, buffer, 1024);

    std::string request(buffer);
    std::istringstream requestStream(request);
    std::string method;
    std::string url;

    requestStream >> method >> url;

    // Search for the end of the HTTP headers
    std::string delimiter = "\r\n\r\n";
    size_t delimiterPos = request.find(delimiter);
    std::string postData;

    if (delimiterPos != std::string::npos) {
        postData = request.substr(delimiterPos + delimiter.length());
    }

    std::cout << "-> buffer:{" << buffer << "}" << std::endl;
    std::cout << "--> postData:{" << postData << "}" << std::endl;

    if (url.find("/cgi-bin/") != std::string::npos) {
        executeCGI("." + url, clientSocket, method, postData);
    }
    else if (url == "/form.html")
        serveForm(clientSocket);
    else {
        // Handle other routes here...
    }

    close(clientSocket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Waiting for connections on port " << PORT << "..." << std::endl;

    while (true) {
        std::cout << "Waiting for connections..." << std::endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        handleClient(new_socket);
    }

    close(server_fd);

    return 0;
}
