
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
	// this->addr.sin_addr.s_addr = INADDR_ANY;
	this->addr.sin_addr.s_addr = inet_addr(host.c_str());
	if (bind(this->fd_socket, (struct sockaddr *)&this->addr, this->addr_size) < 0)
		fatal("bind");
	if (listen(this->fd_socket, SOMAXCONN) < 0)
		fatal("listen");


 // flagged as error for not multiplying maxclient by its element size
	// memset(this->fds, 0, MAX_CLIENT);
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

		// possibly the working new version as of FEB 14th. Loops and is able to deal with multiples recv() calls
void listenner::run()
{
	if (poll(fds, this->n_fd, 100) < 0)
	{
		fatal("poll");
	}
	std::string receivedData;
	for (u_int16_t i = 0; i < this->n_fd; i++)
	{
			    // Set socket to non-blocking mode
		int flags = fcntl(fds[i].fd, F_GETFL, 0);
		if (flags == -1) {
			// Handle error
			perror("fcntl");
		} else {
		if (fcntl(fds[i].fd, F_SETFL, flags | O_NONBLOCK) == -1) {
			// Handle error
			perror("fcntl");
		}


	}
		if (fds[i].revents & POLLIN || fds[i].revents & POLLOUT)	// I think we need to check read AND write according to the pdf
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
				int res;
				while ((res = recv(fds[i].fd, buffer, sizeof(buffer), 0)) > 0) {
					receivedData.append(buffer, res);
				}
				// if (res < 0) // pt faire une erreur 500 ici si on spam :/
				// {
				// 	continue;
				// }
				if (res == 0)
				{
					close(fds[i].fd);  // close est une fonction autorisée!
					fds[i] = fds[n_fd - 1];
					n_fd--;
				}
				else
				{
					if (isChunked(receivedData) || isChunkTest(receivedData))
					{
						receivedData = unchunk(receivedData);
					}
					std::cout << RED << "[DEBUG] [RECV] : \n" << RESET <<  receivedData << std::endl;
					try {
						Req x(receivedData, fds[i].fd, this->_location, *this);

												// cout << "RESPONSE STRING\n" << x.responseString << endl;

						ssize_t bytesSent = send(fds[i].fd, x.responseString.c_str(), x.responseString.length(), 0);
						if ( bytesSent < 0)
						{
							cout << "bytes sent to client: " << bytesSent << endl;
							continue; // même chose quen haut, pt erreur 500, a voir
						}

						// std::cout << RED << "[DEBUG] [SEND] : \n" << RESET <<  x.getHttpString() << std::endl;
					}
					catch (std::exception &e) {
						// std::cout << RED << "[DEBUG] catch: \n" << RESET <<  e.what() << std::endl;
					}
				}
			}
		}
	}
}




					// if (isMulti(receivedData))
					// {
					// 	string boundary = 
					// }
					// if (res > 0)
					// {
					// 	continue;
					// }

				// saving OG
// void listenner::run()
// {
// 	if (poll(fds, this->n_fd, 100) < 0)
// 	{
// 		fatal("poll");
// 	}
// 	std::string receivedData;
// 	for (u_int16_t i = 0; i < this->n_fd; i++)
// 	{
// 		if (fds[i].revents & POLLIN || fds[i].revents & POLLOUT)	// I think we need to check read AND write according to the pdf
// 		{
// 			if (i == 0)
// 			{
// 				this->fds[this->n_fd].fd = client(*this).getfd();
// 				this->fds[this->n_fd].events = POLLIN;
// 				this->n_fd++;
// 			}
// 			else
// 			{
// 				char buffer[0xffff];
// 				int res = recv(fds[i].fd, buffer, 0xffff - 1, 0);
// 				receivedData.append(buffer, res);
// 				if (res < 0) // pt faire une erreur 500 ici si on spam :/
// 				{
// 					continue;
// 				}
// 				else if (res == 0)
// 				{
// 					close(fds[i].fd);  // close est une fonction autorisée!
// 					fds[i] = fds[n_fd - 1];
// 					n_fd--;
// 				}
// 				else
// 				{
// 					if (isChunked(receivedData) || isChunkTest(receivedData))
// 					{
// 						receivedData = unchunk(receivedData);
// 					}
// 					std::cout << RED << "[DEBUG] [RECV] : \n" << RESET <<  receivedData << std::endl;
// 					try {
// 						Req x(receivedData, fds[i].fd, this->_location, *this);

// 												// cout << "RESPONSE STRING\n" << x.responseString << endl;

// 						ssize_t bytesSent = send(fds[i].fd, x.responseString.c_str(), x.responseString.length(), 0);
// 						if ( bytesSent< 0)
// 						{
// 							cout << "bytes sent to client: " << bytesSent << endl;
// 							continue; // même chose quen haut, pt erreur 500, a voir
// 						}

// 						// std::cout << RED << "[DEBUG] [SEND] : \n" << RESET <<  x.getHttpString() << std::endl;
// 					}
// 					catch (std::exception &e) {
// 						// std::cout << RED << "[DEBUG] catch: \n" << RESET <<  e.what() << std::endl;
// 					}
// 				}
// 			}
// 		}
// 	}
// }

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
				cout << " THIS IS A CHUNK TEST " << endl;
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

string	listenner::trimLine(string &line)
{
	while (line.back() == '\n' || line.back() == '\r')
		line.pop_back();
	return line;
}