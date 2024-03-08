/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gmiyakaw <gmiyakaw@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/13 12:18:14 by dluna-lo          #+#    #+#             */
/*   Updated: 2024/03/08 14:28:45 by gmiyakaw         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include "Location.hpp"


// 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <string>
// 


#include "../socket/Listenner/Listenner.hpp"
class Parsing;

class Server
{
private:
    /* data */
    std::string _name;

    std::vector<std::string> _ports;
    std::vector<std::string>::iterator itr_ports;

    std::vector<listenner> _listenners;

    std::string _host;
    std::string _root;
    std::string _index;

    // map
    std::map<std::string, std::string> _error_page;
    std::map<std::string, std::string>::iterator itr_error_page;

    std::vector<std::string> _methods;
    std::vector<std::string>::iterator itr_methods;

    int _body_size;

    std::vector<Location> v_locations;
    std::vector<Location>::iterator itr_locations;
public:
    Server();
    ~Server();

    std::string get_name();

    size_t get_ports_size();
    std::string get_ports();
    std::string get_ports(int index);
    
    std::vector<std::string>& get_ports_ref();
    void restar_itr_port();

    std::string get_host();
    std::string get_root();
    std::string get_index();

    size_t get_error_page_size();
    std::string get_error_page();
    std::string get_error_page(std::string index);
    void restar_itr_error_page();

    size_t get_methods_size();
    std::string get_methods();
    std::string get_methods(int index);
    void restar_itr_methods();

    size_t get_location_size();
    Location get_location();
    Location get_location(int index);
    void restar_itr_location();

    void set_name(std::string name);
    void set_new_port(std::string port);
    void set_body_size(int size);
    void set_host(std::string host);
    void set_root(std::string root);
    void set_index(std::string index);
    void set_new_error_page(std::string index, std::string path);
    void set_new_method(std::string method);
    void set_new_location(Location lo);
    void initPorts(void);

	listenner &get_listenners(int index);
	size_t get_listenners_size();

    void startListeningOnPorts();
};

std::ostream& operator<<(std::ostream& out, Server& src);

#endif