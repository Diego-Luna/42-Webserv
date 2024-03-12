/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dluna-lo <dluna-lo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/13 12:18:16 by dluna-lo          #+#    #+#             */
/*   Updated: 2024/03/12 15:30:01 by dluna-lo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include <string>
#include <vector>

class Location
{
private:
    /* data */
    std::string _name;
    std::string _root;
    std::string _index;
    std::string _upload_folder;
    // bool _autoindex = false;
    std::vector<std::string> _methods;
    std::vector<std::string>::iterator itr_methods;
    bool _autoindex;

public:
    Location();
    ~Location();

    std::string get_name();
    std::string get_root();
    std::string get_index();
    std::string get_upload_folder();
    size_t get_methods_size();
    std::string get_methods();
    std::string get_methods(int index);
    bool get_autoindex();

    void set_name(std::string name);
    void set_root(std::string root);
    void set_index(std::string index);
    void set_upload_folder(std::string _upload_folder);
    void set_new_method(std::string method);
    void set_autoindex(bool autoindex);


    //  reset itrs
    void reset_itr();
};

std::ostream& operator<<(std::ostream& out, Location& src);

#endif