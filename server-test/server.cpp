#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <sys/stat.h>

#define PORT 8080
#define BUFFER_SIZE 4096

void listDirectoryContents(int clientSocket, const char *dirPath) {
    DIR *dir;
    struct dirent *ent;
    char message[BUFFER_SIZE] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body><h1>Directory Listing</h1><ul>";

    if ((dir = opendir(dirPath)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (ent->d_name[0] != '.') { // Ignore hidden files (and parent/current directories)
                strcat(message, "<li><a href=. \"");
                strcat(message, ent->d_name);
                strcat(message, "\">");
                strcat(message, ent->d_name);
                strcat(message, "</a></li>");
            }
        }
        closedir(dir);
    } else {
        // Could not open directory
        perror("Could not open directory");
        return;
    }

    strcat(message, "</ul></body></html>");

    write(clientSocket, message, strlen(message));
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t sin_len = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Cannot open socket");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("bind error");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    listen(serverSocket, 10);

    while (1) {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &sin_len);
        if (clientSocket == -1) {
            perror("Connection failed...\n");
            continue;
        }
        printf("Got connection\n");

        listDirectoryContents(clientSocket, "../data/www/Pages");

        close(clientSocket);
    }

    close(serverSocket);
    return 0;
}
