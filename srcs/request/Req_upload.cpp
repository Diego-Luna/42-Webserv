#include "Req.hpp"

void	Req::parseUpload(void)
{
	string	boundary = findUploadBoundry();
	if (_error == false)
		_fileName = findUploadFileName(boundary);
	if (_error == false)
		_body = findUploadBody(boundary);
	_populateEnv(string("Host"));
	_populateEnv(string("User-Agent"));
	_populateEnv(string("Content-Length"));
	_populateEnv(string("Accept"));
	_populateEnv(string("Accept-Language"));
	_populateEnv(string("Connection"));
	_buildEncoded();
	env["CONTENT_TYPE"] = "text/plain";
	env["SERVER_PROTOCOL"] = _protocol;
	env["FILE_NAME"] = _fileName;
	if (_error == false)
	{
		set_status_code(OK);
	}
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
	size_t bytesWritten = fwrite(_body.c_str(), sizeof(char), _body.length(), uploadFile);
	if (bytesWritten != _body.length())
	{
		std::cerr << "error. could not write all data to file: " << _fileName << endl;
		_error = true;
		set_status_code(INTERNAL_SERVER_ERROR);
		return;
	}
	fclose(uploadFile);
}
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
	while (!_ReqStream.eof() && line.compare(0, boundary.length(), boundary) != 0)
	{
		std::getline(_ReqStream, line);
		line = trimLine(line);
		if (line.compare(0, boundary.length(), boundary) == 0) {
			// body.pop_back();		// removes extraneous \n
			body.erase(body.size() -1);
			break;
		}
		body += line + "\n";
	}
	return body;
}

std::string Req::trimLine(std::string line)
{
	while (!line.empty() && (line.at(line.size() - 1) == '\n' || line.at(line.size() - 1) == '\r'))
	{
		line.erase(line.size() - 1);
	}
	return line;
}

string	Req::findUploadBoundry(void)
{
	string boundary = "boundary=";
	if (_http_Req.find(boundary) == string::npos)
	{
		std::cerr << "Upload error: boundary not found" << endl;
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
	while (it != line.end())
	{
		fileName += *it;
		it++;
	}
	fileName = trimLine(fileName);
	stripQuotes(fileName);
	fileName = PATH_TO_UPLOAD + fileName;
	return fileName;
}

void Req::stripQuotes(string &original)
{
	if (!original.empty() && original[0] == '"')
	{
		original.erase(0, 1);
	}
	if (!original.empty() && original[original.size() - 1] == '"')
	{
		original.erase(original.size() - 1);
	}
}
