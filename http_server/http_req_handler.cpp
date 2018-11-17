#include "http_req_handler.h"

using namespace std;

void http_req_handler::parse_request() {

    valid = 0;  //default as invalid request

    //break the request string into lines
    vector<string> lines = yaws::split_text(request, CRLF);
    if(lines.size() == 0) {
        return; //malformed request
    }

    /* handle the first line */
    vector<string> fline_parameters = yaws::split_text(lines[0], " ");
    if(fline_parameters.size() != 3) {
        return; //malformed request
    }

    //get data out of the first line
    http_method = get_http_method_pv(fline_parameters[0]);
    http_version = get_http_version_pv(fline_parameters[2]);
    target = fline_parameters[1];

    size_t qm_index = target.find_last_of("?");
    if(qm_index != string::npos) {
        target = target.substr(0, qm_index);
    }

    /* handle the headers of the request */
    for(int i=1; i<lines.size(); i++) {
        vector<string> type_val = yaws::split_text(lines[i], ": ");
        if(type_val.size() != 2){
            return; //malformed head
        }
        headers[type_val[0]] = type_val[1];
    }

    //done with parsing as success
    valid = 1;
    return;
}

HTTP_METHOD http_req_handler::get_http_method_pv(const std::string& method_str){
    if (method_str == "GET") {
        return HTTP_METHOD_GET;
    } else if (method_str == "POST") {
        return HTTP_METHOD_POST;
    }
    return HTTP_METHOD_NOT_IMPLEMENTED;
}

HTTP_VERSION http_req_handler::get_http_version_pv(const std::string& version_str){
    if (version_str == "HTTP/1.0") {
        return HTTP_VERSION_HTTP1_0;
    } else if (version_str == "HTTP/1.1") {
        return HTTP_VERSION_HTTP1_1;
    }
    return HTTP_VERSION_HTTP_UNSUPPORTED;
}

http_req_handler::http_req_handler(const std::string& request){
    this->request = request;
    parse_request();
}

http_req_handler::~http_req_handler(){
    //nothing to do here
}

HTTP_METHOD http_req_handler::get_http_method(){
    return http_method;
}

HTTP_VERSION http_req_handler::get_http_version(){
    return http_version;
}

std::string http_req_handler::get_http_url_target(){
    return target;
}

std::unordered_map<std::string, std::string> http_req_handler::get_headers(){
    return headers;
}

std::string http_req_handler::get_header_value(const std::string& header_type){
    if(headers.find(header_type) != headers.end()) {
        return headers[header_type];
    }
    return "";
}