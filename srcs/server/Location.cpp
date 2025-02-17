#include "Location.hpp"

Location::Location()
{
  this->itr_methods = this->_methods.begin();

  this->_name = "";
  this->_root = "";
  this->_index = "";
  this->_upload_folder = "";
  this->_autoindex = true;
}

Location::~Location()
{
}

void Location::reset_itr()
{
  this->itr_methods = this->_methods.begin();
}

std::string Location::get_name(){
  return this->_name;
}
std::string Location::get_root(){
  return this->_root;
}

std::string Location::get_index(){
  return this->_index;
}

std::string Location::get_upload_folder(){
  return this->_upload_folder;
}

std::string Location::get_methods(){
  std::string methods = *this->itr_methods;
  if (this->itr_methods == this->_methods.end())
    this->itr_methods = this->_methods.begin();
  else
    this->itr_methods++;
  return methods;
}
std::string Location::get_methods(int index){
  if (index < 0 || (size_t)index > this->_methods.size())
  {
    throw std::invalid_argument("Index out of range");
  }

  return this->_methods[index];
}

bool Location::get_autoindex(){
  return this->_autoindex;
}


void Location::set_name(std::string name){
  this->_name = name;
}

void Location::set_root(std::string root){
  this->_root = root;
}

void Location::set_index(std::string index){
  this->_index = index;
}

void Location::set_upload_folder(std::string _upload_folder){
  this->_upload_folder = _upload_folder;
}

void Location::set_new_method(std::string method){
  this->_methods.push_back(method);
}

void Location::set_autoindex(bool autoindex){
  this->_autoindex = autoindex;
}

size_t Location::get_methods_size(){
  size_t size = this->_methods.size();
  return (size);
}

// ostream
std::ostream& operator<<(std::ostream& out, Location& src) {
    out << "Location name:" << src.get_name() << "\n";
    out << "Location root:" << src.get_root() << "\n";
    out << "Location index:" << src.get_index() << "\n";
    out << "Location upload_folder:" << src.get_upload_folder() << "\n";

    for (size_t i = 0; i < src.get_methods_size(); i++)
    {
      out << "Location methods:" << src.get_methods((int)i) << "\n";
    }

    out << "Location autoindex:" << src.get_autoindex() << "\n";
    return out;
}
