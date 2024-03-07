/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmiyakaw <gmiyakaw@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/09 13:22:29 by dluna-lo          #+#    #+#             */
/*   Updated: 2024/03/07 11:15:17 by gmiyakaw         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Includes/Parsing.hpp"
#include "request/Req.hpp"

std::vector<std::pair<std::string, std::string> > Req::mime;

void Req::innitMime(void)
{
	std::string list(".aac audio/aac .abw application/x-abiword .arc application/octet-stream .avi video/x-msvideo .azw application/vnd.amazon.ebook \
	.bin application/octet-stream .bmp image/bmp .bz application/x-bzip .bz2 application/x-bzip2 .csh \
	application/x-csh .css text/css .csv text/csv .doc application/msword .docx application/vnd.openxmlformats-officedocument.wordprocessingml.document \
	.eot application/vnd.ms-fontobject .epub application/epub+zip .gif image/gif .htm text/html .html text/html .ico image/x-icon .ics text/calendar \
	.jar application/java-archive .jpeg image/jpeg .jpg image/jpeg .js application/javascript .json application/json .mid audio/midi .midi audio/midi \
	.mpeg video/mpeg .mpkg application/vnd.apple.installer+xml .odp application/vnd.oasis.opendocument.presentation .ods application/vnd.oasis.opendocument.spreadsheet \
	.odt application/vnd.oasis.opendocument.text .oga audio/ogg .ogv video/ogg .ogx application/ogg .otf font/otf .png image/png .pdf application/pdf \
	.ppt application/vnd.ms-powerpoint .pptx application/vnd.openxmlformats-officedocument.presentationml.presentation .rar application/x-rar-compressed \
	.rtf application/rtf .sh application/x-sh .svg image/svg+xml .swf application/x-shockwave-flash .tar application/x-tar .tif image/tiff \
	.tiff image/tiff .ts application/typescript .ttf font/ttf .vsd application/vnd.visio .wav audio/x-wav .weba audio/webm .webm video/webm \
	.webp image/webp .woff font/woff .woff2 font/woff2 .xhtml application/xhtml+xml .xls application/vnd.ms-excel \
	.xlsx application/vnd.openxmlformats-officedocument.spreadsheetml.sheet .xml application/xml .xul application/vnd.mozilla.xul+xml \
	.zip application/zip .3gp video/3gpp .3g2 video/3gpp2 .7z application/x-7z-compressed .py application/x-www-form-urlencoded \
	.txt text/plain");
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
		std::cerr << "-> file" << std::endl;
		parsing.checkData(argv[1]);
		if (parsing.check_error())
			return (1);
		parsing.saveData(argv[1]);
		if (parsing.check_error())
			return (1);
	}
	else {
		std::cerr << "-> run default" << std::endl;
		parsing.saveData("run default");
		if (parsing.check_error())
			return (1);
		parsing.seeData();					// commenting this line segfaults Req parsing for some reason
		if (parsing.check_error())
			return (1);
	}

	parsing.seeData();

	Req::innitMime();

	for (size_t i = 0; i < parsing.get_server_size(); i++)
	{
		parsing.get_ref_server(i).initPorts();
	}


	while (true) {
    for (size_t i = 0; i < parsing.get_server_size(); i++) {
      Server &server = parsing.get_ref_server(i);
      for (size_t j = 0; j < server.get_ports_size(); j++) {
				// server.get_listenners(j).run();
				server.get_listenners(j).run(server);
			}
		}
	}

				cout << "back to main from get_listeners.run()" << endl;

	// for (size_t i = 0; i < parsing.get_server_size(); i++) {
  //   Server &server = parsing.get_ref_server(i);
  //   server.startListeningOnPorts();
	// }


	return 0;
}
