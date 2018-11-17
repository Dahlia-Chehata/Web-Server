#include <string>
#include <iostream>

#include "types_manager.h"

using namespace std;

string types_manager::get_file_type (const string& file_name) {

  size_t dot_index = file_name.find_last_of("."), i;
  if(dot_index == string::npos) {
  	return "NOT SUPPORTED DATA TAYPE";
  }

  //to_lower
  string type = file_name.substr(dot_index+1);
  for(i=0; i<type.length(); i++) {
    if(type[i] >= 'A' && type[i] <= 'Z') {
      type[i] = type[i] - 'A' + 'a';
    }
    //start of parameters
    if(type[i] == '?') {
      break;
    }
  }

  return type.substr(0, i);
}

bool types_manager::is_file_supported(const string& file_name) {

  string type = get_file_type(file_name);
  
  if(type == "txt" ||
  	 type == "jpg" ||
  	 type == "gif" ||
  	 type == "html"||
     type == "js"  ||
     type == "css" ||
     type == "png" ||
     type == "ttf") {

  	return true;
  }

  return false;
}

string types_manager::generate_file_header(const string& file_name) {

  string type = get_file_type(file_name);

  if(type == "html") {
  	return "Content-Type: text/html";
  }

  if(type == "css") {
    return "Content-Type: text/css";
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

  if(type == "ttf") {
    return "Content-Type: image/ttf";
  }

  if(type == "png") {
    return "Content-Type: image/png";
  }

  if(type == "js") {
    return "Content-Type: application/javascript";
  }

  return "";
}
