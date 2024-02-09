#include "Req.hpp"

void	Req::parseUpload(void)
{
								cout << "start of parsing upload" << endl;
	
	string	boundary = findUploadBoundry();
					cout << "boundary: |" << boundary << "|" << endl;

	if (_error == false)
		_fileName = findUploadFileName(boundary);

					cout << "fileName: |" << _fileName << "|" << endl;

	if (_error == false)
		_body = findUploadBody(boundary);

					cout << "body: |" << _body << "|" << endl;

	_populateEnv(string("Host"));
	_populateEnv(string("User-Agent"));
	_populateEnv(string("Content-Length"));
	_populateEnv(string("Accept"));
	_populateEnv(string("Accept-Language"));
	_populateEnv(string("Connection"));
	_buildEncoded();
	env["CONTENT_TYPE"] = "text/plain";
	env["SERVER_PROTOCOL"] = _protocol;
	env["FILE_NAME"] = _fileName;	// might change it to uploadfilenam

	set_status_code(OK);
}

void	Req::createUploadFile()
{
	FILE	*uploadFile = fopen(_fileName.c_str(), "w");

	if (uploadFile == NULL)
	{
		std::cerr << "error. could not create file: " << _fileName << endl;
		_error = true;
		set_status_code(INTERNAL_SERVER_ERROR);
		return;
	}
							cout << "just before file creation\n\n" << endl;
	size_t bytesWritten = fwrite(_body.c_str(), sizeof(char), _body.length(), uploadFile);
						cout << "just after file creation\n\n" << endl;
						cout << "bytes written: " << bytesWritten << endl;
	fclose(uploadFile);
}
		// WORKING FOR TXT FILES -> NEW VERSION IN PROGRESS
	// picks up the stream from where findUploadFileName left off
string	Req::findUploadBody(string boundary)
{
	boundary = trimLine(boundary);
	boundary = "--" + boundary + "--";

	string line;
	string body = "";
	
	std::getline(_ReqStream, line);
	line = trimLine(line);
	while (!line.empty())
	{
		std::getline(_ReqStream, line);
		line = trimLine(line);
	}
					cout << "after first loop: " << line << endl;
					cout << "boundary: |" << boundary << "|" << endl;
					cout << line.compare(0, boundary.length(), boundary) << endl;

		// working, but could use some cleanup.
	while (!_ReqStream.eof() && line.compare(0, boundary.length(), boundary) != 0)
	{
		std::getline(_ReqStream, line);
		line = trimLine(line);
		if (line.compare(0, boundary.length(), boundary) == 0) {
			body.pop_back();		// removes extraneous \n
			break;
		}
		body += line + "\n";
	}
	return body;
}


// 	// picks up the stream from where findUploadFileName left off
// string Req::findUploadBody(string boundary)
// {
//     boundary = trimLine(boundary);
//     boundary = "--" + boundary;
//     boundary = boundary + "--";

//    				 cout << "boundary:\n" << boundary << endl;
//     // advances to the start of the body
// 	string tmpLine = "";
// 				cout << " line pre loop: " << tmpLine << endl;
// 	while (getline(_ReqStream, tmpLine) && tmpLine != "\r"){
// 	}
// 				cout << " line post loop: " << tmpLine << endl;
//     size_t bodyStart = static_cast<size_t>(_ReqStream.tellg());
// 				cout << "bodystart: " << bodyStart << endl;

//     // makes a new vector that starts from the point after the start boundary
// 		std::vector<char> bodyVector;
// 		bodyVector.clear();
// 		char ch;
// 		while (_ReqStream.get(ch)) {
// 			bodyVector.push_back(ch);
// 		}


// 	// find the position of the boundary in the vector
// 	std::vector<char>::iterator boundaryPos = std::search(bodyVector.begin(), \
// 							bodyVector.end(), boundary.begin(), boundary.end());

						

//     // remove the boundary if found
//     if (boundaryPos != bodyVector.end())
//     {
//         bodyVector.erase(boundaryPos, boundaryPos + boundary.size());
//     }

//     for (std::vector<char>::iterator it = bodyVector.begin(); it != bodyVector.end(); ++it)
//     {
//         std::cout << *it;
//     }
//     std::cout << std::endl;

// 							// cout << " just before setting _bodyvector" << endl;

// 	_bodyVector = bodyVector;
// 							cout << dataVector.size() << " data vector size" << endl;
// 							cout << "header size " << _header.size() << endl;
// 							cout << _bodyVector.size() << " body vector size" << endl;

//     string body(bodyVector.begin(), bodyVector.end());
// 							cout << " just after setting _bodyector" << endl;
//     return body;
// }





string	Req::trimLine(string line)
{
	while (line.back() == '\n' || line.back() == '\r')
		line.pop_back();
	return line;
}

string	Req::findUploadBoundry(void)
{
	string boundary = "boundary=";
	if (_http_Req.find(boundary) == string::npos)
	{
								cout << "BOUNDRY NOT FOUND" << endl;
		_error = true;
		set_status_code(BAD_REQUEST);
		return "";
	}
	string::iterator it = _http_Req.begin() + _http_Req.find(boundary) + boundary.length();
	boundary = "";
	while (*it != '\n' && *it != '\r' && *it != ' ')
	{
		boundary += *it;
		it++;
	}

	return boundary;
}
		// KNOWN BUG: filename can't have a space in it.

string	Req::findUploadFileName(string boundary)
{
	string	fileName = "";
	string	line;

		// Reset the stream to the beginning
	_ReqStream.clear();            // Clear any error flags
	_ReqStream.seekg(0, _ReqStream.beg); // Set the position to the beginning

	boundary = "--" + boundary;
	
	std::getline(_ReqStream, line);
	while (!line.empty() && line.compare(0, boundary.length(), boundary) != 0)
		std::getline(_ReqStream, line);
	std::getline(_ReqStream, line);

				cout << "line after boundary?\n" << line << endl;
	
	string	tmp = "Content-Disposition:";
	while (!line.empty() && line.compare(0, tmp.length(), tmp) != 0)
		std::getline(_ReqStream, line);
	if (line.find("filename=") == string::npos)
	{
		_error = true;
		set_status_code(BAD_REQUEST);
		return "";
	}
	string::iterator it = line.begin() + line.find("filename=") + 9;
	while (it != line.end() && *it != ' ')
	{
		fileName += *it;
		it++;
	}
	fileName = trimLine(fileName);
	stripQuotes(fileName);
	fileName = PATH_TO_UPLOAD + fileName;
	return fileName;
}

void	Req::stripQuotes(string &original)
{
	if (original.front() == '"')
		original.erase(0, 1);
	if (original.back() == '"')
		original.pop_back();
	return;
}
