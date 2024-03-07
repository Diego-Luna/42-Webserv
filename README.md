

<p align="center">
    <a href="https://www.cprogramming.com/"><img alt="Made with C++" src="https://img.shields.io/badge/Made%20with-C++-1f425f.svg"/></a>
    <img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/bylkuss/Webserv?color=lightblue" />
    <img alt="Number of lines of code" src="https://img.shields.io/tokei/lines/github/bylkuss/Webserv?color=critical" />
    <img alt="Code language count" src="https://img.shields.io/github/languages/count/bylkuss/Webserv?color=yellow" />
    <img alt="GitHub top language" src="https://img.shields.io/github/languages/top/bylkuss/Webserv?color=blue" />
    <img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/bylkuss/Webserv?color=green" />
</p>

# 42 Webserv Project 📝  
Import your existing Readme using the import button on the top right corner of the navbar, 
or create a new Readme from scratch by typing in the editor.  

## Usage 🚀
To run the server, execute the following command:

```
make run [configuration file]
```

## Requirements
* The server must be non-blocking and use only one poll (or equivalent) for all I/O operations, including the client-server communication and listening.
* The server should properly handle client disconnection and ensure non-blocking behavior.
* All read and write operations should go through the poll (or equivalent) mechanism, and checking the errno value after a read or write operation is strictly forbidden.
* The configuration file should be read without using poll (or equivalent).
* The server should be compatible with a web browser, and its HTTP response status codes must be accurate.
* Default error pages should be provided if none are specified.
* The server should support serving a fully static website, accepting GET, POST, and DELETE methods.
* File uploading by clients should be supported.
* Stress testing should be performed to ensure the server remains available at all times.
* The server must be able to listen to multiple ports as specified in the configuration file.


## Configuration File
The configuration file should include the following options for each server:

* Port and host selection.
* Definition of server names.
* Setting default error pages.
* Limiting client body size.
* Configuring routes with various rules, including accepted HTTP methods, HTTP redirection, file or directory serving, directory listing, default file for directories, CGI execution based on file extension, handling POST and GET methods, and configuring file upload destinations.
Please refer to the provided configuration files and default basic files for testing and demonstration purposes.

Before starting the project, it is recommended to read the relevant RFC and perform tests using telnet and NGINX to gain a better understanding of HTTP and ensure compliance.

Note: For MacOS, use `fcntl()` with the `F_SETFL` flag set to `O_NONBLOCK` for achieving non-blocking behavior with file descriptors.

Feel free to explore and contribute to this HTTP server implementation.

## For run CGI Test

```
g++ -o web_server web_server.cpp -lpthread && ./web_server
```
and the website : http://localhost:8080/form.html




***********************************************
 <footer>
    <p align=center style="float:left; width: 20%;">
        &copy; Lounes Adjou, Diego-Luna, 2023
    </p>
    <p align=center style="float:left; width: 20%;">
        For inquiries, please email me at: <a href="mailto:lounes.adjou@gmail.com">email</a>
    </p>
</footer>

 ***********************************************



