#ifndef CGI_HPP
#define CGI_HPP


#include "request/Req.hpp"
#include "../Includes/Parsing.hpp"

class CGI {

private:
	CGI();

public:

	~CGI();
	CGI(Req &req_);
	
	void m_setEnv();
     
	std::string	m_scriptGet();
	std::string m_httpRequestGet();
	std::string m_pathInfoGet();
	std::string m_queryStringGet();
	std::string m_headerGet(std::string header_);

	void	exec();

	Req& 					_req;
	std::vector<std::string>	_env;
	
	// shouldnt it be a reference to the server that called it?
	Server			m_server;


};



/*
The common gateway interface is a standard protocol
for enabling applications (called CGi scripts or programs)
to interact with web servers and clients.
theses CGi programs can be written Python, PERL, C, C++ etc

see if this makes sense:

hey, rubber duck with me for a bit. I could use your input.

This is what I'm currently thinking it will look like:
We're building CGI script/program as a class.

assuming this workflow:

server gets request, validates it -> creates a request ->
request calls CGI -> CGI runs and returns output to server.

to build and run it we need:

a populated environment:
		variables populated by the webserver before calling CGI.
		these include REQUEST_METHOD PATH_INFO etc

A reference or copy of the request itself:
		so that the info may be validaded before executing.
		may be unecessary if it's already checked during parsing or request validation.

A reference to the server:
		so that we may return the output to it.
		may be unnecessary if the request already has a variable for it (I could not find it)

Which script to run:
		This is where it gets tricky. We could decide it before
		calling CGI, passing it as an argument to the construtor perhaps
		or
		have a way to sort it out from within the class. Using the information
		from the request. Is that information already parsed somewhere before?


for example:
lets say we're processing a POST request.
It's a message box that will be filled with the string "Hello! How are you?"

I imagine it would run something like this once it gets to the request:

	request.setEnvironment(); 
	CGI cgi(request, server);
		cgi.runScript();
		cgi.outputToServer();
	
That's roughly what I think the order of things should be.
what I'm missing from this logic is:

		how do we identify and catalog the scripts?
	should it be just parsed as conditions
	as in:
	
		if request.getMethod() == GET && URL == www.index.html/form
			cgi.run(getScript);
		else if request.getMethod() == POST && URL == www.index.html/form
			cgi.run(postScript);
		... etc
		
	or, the way I found more commonly online, is having each script as a
	separate file in the server. we would populate the environment variable
	SCRIPT_NAME prior to calling CGI and that would just be the path to 
	the script being run.
	as in:
	
		if (cgi.checkPath(SCRIPT_NAME) == true) {
			cgi.run(SCRIPT_NAME);
		}
		
		
I'm more partial to the second method, as it's closer to what I've seen done 
as the standard. the only difference I see in our current code is that we're building
the environment ourselves for each request and most places seem to just set it
as a true environment variable using std::setenv() and std::getenv();

But it does come with the cost of having to parse that on Request, but I do think it makes
more sense there.

But I don't know if that choice was made for any particular reason, ie. we're not allowed
to use global variables and the like or if that workflow wouldn't work for any reason.

which is why I come to you.
	any thoughts?

*/

#endif