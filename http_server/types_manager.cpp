#include <string>
#include <iostream>

#include "types_manager.h"

using namespace std;

string types_manager::get_file_type (const string& file_name) {

  size_t dot_index = file_name.find_last_of(".");
  if(dot_index == string::npos) {
  	return "NOT SUPPORTED DATA TAYPE";
  }

  return file_name.substr(dot_index+1);
}

bool types_manager::is_file_supported(const string& file_name) {

  string type = get_file_type(file_name);

  if(type == "txt" ||
  	 type == "jpg" ||
  	 type == "gif" ||
  	 type == "html") {

  	return true;
  }

  return false;
}

string types_manager::generate_file_header(const string& file_name) {

  string type = get_file_type(file_name);

  if(type == "html") {
  	return "Content-Type: text/html";
  }

  if(type == "txt") {
  	return "Content-Type: text/plain";
  }

  if(type == "jpg") {
  	return "Content-Type: image/jpeg";
  }

  if(type == "gif") {
  	return "Content-Type: image/gif";
  }

  return "";
}
