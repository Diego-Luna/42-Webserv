#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <cstdlib>
#include <cstdio> // Para fprintf
#include <cstring> // Para strlen


const int PORT = 8080;

extern char **environ;  // Declaración de la variable environ

// Asumiendo que ya tienes el resto de las inclusiones y tu función executeCGI comienza aquí

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

    if (pid == 0) { // Proceso hijo
        close(pfd[0]); // Cerrar el extremo de lectura del pipe de salida
        close(input_pfd[1]); // Cerrar el extremo de escritura del pipe de entrada

        dup2(pfd[1], STDOUT_FILENO); // Duplicar el extremo de escritura del pipe de salida en stdout
        dup2(input_pfd[0], STDIN_FILENO); // Duplicar el extremo de lectura del pipe de entrada en stdin

        close(pfd[1]); // Cerrar el duplicado
        close(input_pfd[0]); // Cerrar el duplicado

        setenv("REQUEST_METHOD", method.c_str(), 1);
        if (method == "POST") {
            // Establecer CONTENT_LENGTH como variable de entorno
            std::string contentLengthStr = std::to_string(postData.length());
            setenv("CONTENT_LENGTH", contentLengthStr.c_str(), 1);
        }

        const char* argv[] = { "python3", scriptPath.c_str(), NULL };
        execve("/usr/bin/python3", const_cast<char* const*>(argv), environ);

        perror("execve failed");
        exit(EXIT_FAILURE);
    } else { // Proceso padre
        close(pfd[1]); // Cerrar el extremo de escritura del pipe de salida
        close(input_pfd[0]); // Cerrar el extremo de lectura del pipe de entrada

        // Si el método es POST, escribir los datos POST en el pipe de entrada
        if (method == "POST") {
            write(input_pfd[1], postData.c_str(), postData.length());
        }

        close(input_pfd[1]); // Cerrar el extremo de escritura después de enviar los datos

        // Leer la salida del script CGI del pipe de salida y enviarla al cliente
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
    // Ruta al archivo HTML que quieres servir
    std::string filePath = "./form.html";

    // Abre el archivo en modo de lectura
    std::ifstream fileStream(filePath, std::ios::binary);

    if(fileStream) {
        // Lee el contenido del archivo
        std::string httpResponse = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
        std::stringstream buffer;
        buffer << fileStream.rdbuf();

        // Agrega el contenido del archivo a la respuesta HTTP
        httpResponse += buffer.str();

        // Envía la respuesta al cliente
        send(clientSocket, httpResponse.c_str(), httpResponse.length(), 0);
    } else {
        // Si no se encuentra el archivo, envía una respuesta de error 404
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

    // Buscar el final de las cabeceras HTTP
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
        // Manejar otras rutas aquí...
    }

    close(clientSocket);
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    // Crear socket
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

    std::cout << "Esperando conexiones en el puerto " << PORT << "..." << std::endl;

    while (true) {
        std::cout << "Esperando conexiones..." << std::endl;
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        handleClient(new_socket);
    }

    close(server_fd);


    return 0;
}
