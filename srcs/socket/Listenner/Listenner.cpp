
#include "Listenner.hpp"
#include "../../request/Req.hpp"

void listenner::init(u_int32_t port, std::string host)
{
	u_int32_t e = 1;
	this->fd_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_socket == -1)
		fatal("socket");
	if (setsockopt(this->fd_socket, SOL_SOCKET, SO_REUSEADDR, &e, sizeof(e)) < 0)
		fatal("setsockopt");
	this->addr.sin_family = AF_INET;
	this->addr.sin_port = htons(port);
	this->addr.sin_addr.s_addr = inet_addr(host.c_str());
	if (bind(this->fd_socket, (struct sockaddr *)&this->addr, this->addr_size) < 0)
		fatal("bind");
	if (listen(this->fd_socket, SOMAXCONN) < 0)
		fatal("listen");
	memset(this->fds, 0, MAX_CLIENT * sizeof(this->fds[0]));
	this->fds[0].fd = this->getfd();
	this->fds[0].events = POLLIN | POLLOUT;
	this->n_fd = 1;

}

listenner::listenner(u_int32_t port, Location &location, std::string host) : portNumber(port), _location(location)
{
	std::cout << "host: " << host << " port :" <<  port << std::endl;
	init(port, host);
}

listenner::listenner()
{
	init(DEFAULT_PORT, "127.0.0.1");
}

listenner::~listenner()
{}

void listenner::run(Server _server)
{
	if (poll(fds, this->n_fd, 100) < 0)
	{
		fatal("poll");
	}
	std::string receivedData;
	for (u_int16_t i = 0; i < this->n_fd; i++)
	{

		int flags = fcntl(fds[i].fd, F_GETFL, 0);
		if (flags == -1) {
			std::cerr << "fcntl" << endl;
			internalError(fds[i].fd);
			continue;
		} else {
			if (fcntl(fds[i].fd, F_SETFL, flags | O_NONBLOCK) == -1) {
				std::cerr << "fcntl" << endl;
				internalError(fds[i].fd);
				continue;
			}
		}
		if (fds[i].revents & (POLLIN | POLLOUT))
		{
			if (i == 0)
			{
				this->fds[this->n_fd].fd = client(*this).getfd();
				this->fds[this->n_fd].events = POLLIN;
				this->n_fd++;
			}
			else
			{
				char buffer[BUFFER_SIZE];
				memset(buffer, 0, BUFFER_SIZE);
				int res;
				while ((res = recv(fds[i].fd, buffer, BUFFER_SIZE, 0)) > 0) {
					receivedData.append(buffer, res);
				}
				if (receivedData.empty())	//	solves the false reload issue.
					continue;
				if (res == 0)
				{
					close(fds[i].fd);
					fds[i] = fds[n_fd - 1];
					n_fd--;
				}
				else
				{
					if (isChunked(receivedData) || isChunkTest(receivedData))
						receivedData = unchunk(receivedData);
					if (isChunkTest(receivedData)) {
						cout << "printing unchunked data" << endl;
						cout << receivedData << endl;
						continue;
					}
					// std::cout << RED << "[DEBUG] [RECV] : \n" << RESET <<  receivedData << std::endl;
					try {
						Req request(_server, receivedData, fds[i].fd, this->_location, *this);

										std::cout << RED << "[DEBUG] [RECV] : \n" << RESET <<  request.responseString << std::endl;


						ssize_t bytesSent = send(fds[i].fd, request.responseString.c_str(), request.responseString.length(), 0);
						if ( bytesSent < 0)
						{
							cout << "bytes sent to client: " << bytesSent << endl;
							internalError(fds[i].fd);
							continue; 
						}
						// std::cout << RED << "[DEBUG] [SEND] : \n" << RESET <<  request.getHttpString() << std::endl;
					}
					catch (std::exception &e) {
						// std::cout << RED << "[DEBUG] catch: \n" << RESET <<  e.what() << std::endl;
					}
				}
			}
		}
	}
}

string	listenner::unchunk(const string &receivedData) {
	std::stringstream rawRequest(receivedData);
	string line = "placeholder";
	string unchunked = "";
	while (std::getline(rawRequest, line) && line != "\r")
	{
		unchunked += line;
		unchunked += "\r\n";
	}
	unchunked += "\r\n";
	size_t length;
	do {
		std::getline(rawRequest, line);
		trimLine(line);
		std::stringstream ss;
		ss << std::hex << line;
		ss >> length;
		if (length == 0)
			break;
		std::getline(rawRequest, line);
		trimLine(line);
		unchunked.append(line, 0, length); // appends length characters, counted from the start, to unchunked
	} while (rawRequest.eof() == false);
	unchunked += "\r\n";
	return unchunked;
}

			// for testing purposes only
bool	listenner::isChunkTest(const string &httpRequest) {
	size_t pos = httpRequest.find("chunk test");
	if (pos == string::npos)
		return false;
	else {
		return true;
	}
}

bool	listenner::isChunked(const string &httpRequest) {
	size_t pos = httpRequest.find("Transfer-Encoding: chunked");
	if (pos == string::npos)
		return false;
	else
		return true;
}

bool	listenner::isMulti(const string &httpRequest) {
	size_t pos = httpRequest.find("Content-Type: multipart/form-data");
	if (pos == string::npos)
		return false;
	else
		return true;
}


string listenner::trimLine(string& line)
{
    while (!line.empty() && (line[line.size() - 1] == '\n' || line[line.size() - 1] == '\r'))
    {
        line.erase(line.size() - 1);
    }
    return line;
}



	// changed to comply with c++ 98
// string	listenner::trimLine(string &line)
// {
// 	while (line.back() == '\n' || line.back() == '\r')
// 		line.pop_back();
// 	return line;
// }

// redirects to error 500 page.
void	listenner::internalError(int clientFd) {
	string header = "";

	header += "HTTP/1.1 500 Internal Server Error";

	std::fstream htmlFile("data/www/Pages/ErrorPages/500internalServerError.html");
	string line;
	string responseBody;
	while (std::getline(htmlFile, line))
		responseBody += line + "\r\n";
	htmlFile.close();
	header += "Content-Type: " "text/html";
	header += "\r\n";
	header += "Content-Length: ";
	header +=  size_tToString(responseBody.length());
	header += "\r\n\r\n";
	header += responseBody;
	header += "\r\n";
	send(clientFd, header.c_str(), header.length(), 0);
}
