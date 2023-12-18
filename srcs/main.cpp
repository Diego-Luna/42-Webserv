/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmiyakaw <gmiyakaw@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 13:22:29 by dluna-lo          #+#    #+#             */
/*   Updated: 2023/12/18 10:06:49 by gmiyakaw         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Parsing.hpp"
#include "request/Req.hpp"


std::vector<std::pair<std::string, std::string> > Req::mime;

void Req::innitMime(void)
{
	std::string list(".aac audio/aac .abw application/x-abiword .arc application/octet-stream .avi video/x-msvideo .azw application/vnd.amazon.ebook .bin application/octet-stream .bmp image/bmp .bz application/x-bzip .bz2 application/x-bzip2 .csh application/x-csh .css text/css .csv text/csv .doc application/msword .docx application/vnd.openxmlformats-officedocument.wordprocessingml.document .eot application/vnd.ms-fontobject .epub application/epub+zip .gif image/gif .htm text/html .html text/html .ico image/x-icon .ics text/calendar .jar application/java-archive .jpeg image/jpeg .jpg image/jpeg .js application/javascript .json application/json .mid audio/midi .midi audio/midi .mpeg video/mpeg .mpkg application/vnd.apple.installer+xml .odp application/vnd.oasis.opendocument.presentation .ods application/vnd.oasis.opendocument.spreadsheet .odt application/vnd.oasis.opendocument.text .oga audio/ogg .ogv video/ogg .ogx application/ogg .otf font/otf .png image/png .pdf application/pdf .ppt application/vnd.ms-powerpoint .pptx application/vnd.openxmlformats-officedocument.presentationml.presentation .rar application/x-rar-compressed .rtf application/rtf .sh application/x-sh .svg image/svg+xml .swf application/x-shockwave-flash .tar application/x-tar .tif image/tiff .tiff image/tiff .ts application/typescript .ttf font/ttf .vsd application/vnd.visio .wav audio/x-wav .weba audio/webm .webm video/webm .webp image/webp .woff font/woff .woff2 font/woff2 .xhtml application/xhtml+xml .xls application/vnd.ms-excel .xlsx application/vnd.openxmlformats-officedocument.spreadsheetml.sheet .xml application/xml .xul application/vnd.mozilla.xul+xml .zip application/zip .3gp video/3gpp .3g2 video/3gpp2 .7z application/x-7z-compressed");
	std::vector<std::string> mf = split(list, ' ');

	for (size_t i = 0; i < mf.size(); i += 2)
		Req::mime.push_back(std::make_pair(mf[i], mf[i + 1]));
}


int main(int argc, char const *argv[])
{
	if (argc > 2)
	{
		// TODO: set to default config file when no args.
		std::cerr << "Error: only one or two input." << std::endl;
		return (1);
	}
	Parsing parsing;

	if (argc == 2) {
		parsing.checkData(argv[1]);
		if (parsing.check_error())
			return (1);
		parsing.saveData(argv[1]);
		if (parsing.check_error())
			return (1);

		// Req::innitMime();

		// parsing.get_ref_server(0).initPorts();

		// Location tmp = parsing.get_ref_server(0).get_location(0);

		// while (1)
		// {
		// 	for (size_t i = 0 ;i < parsing.get_ref_server(0).get_listenners_size(); i++)
		// 	{
		// 		parsing.get_ref_server(0).get_listenners(i).run();
		// 	}
		// }

		// while (1) // infinite running for servers
		// 	for (size_t i = 0; i < parsing.get_server_size(); i++) // get each server
		// 		for (size_t j = 0; parsing.get_server(i).get_ports_size(); j++) // get each server's port
		// 				parsing.get_server(i).get_listenners(j).run();
	}
	else {
		parsing.saveData("run default");
		if (parsing.check_error())
			return (1);
		parsing.seeData();					// commenting this line segfaults Req parsing for some reason
		if (parsing.check_error())
			return (1);
	}

	// parsing.seeData();

	// testing req parsing
	(void)argc;
	(void)argv;
	Location	testLocation;
	Server		server;
	string testString;
	int	testFd = open("./mockHttpRequest.txt", O_RDONLY);
	if (testFd < 0) {
		cout << "count not open file" << endl;
		exit(1);
	}
	ssize_t	bytesRead;
	char	buffer[4096];
	while ((bytesRead = read(testFd, buffer, 4096)) > 0) {
		testString.append(buffer, bytesRead);
	}
						// printf("buffer: \n|%s|\n", buffer);
						// printf("testFd: %d\n", testFd);
						// cout << testString << endl;
	cout << "calling Req\n";
	Req			testReq(testString, testFd, testLocation);
	testReq.printReq();

	return 0;
}
