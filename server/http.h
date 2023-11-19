#pragma once

#include <windows.h>
#include <string>
#include <iostream>

#include "page_linker.h"
#include "files.h"

#define HTTP_VER "HTTP/1.1"
#define BUFF_SIZE 5012

//container for data in response, ie. response body
/*struct resp_body {
    char data[BUFF_SIZE];
    size_t bytes;
}; */

//key-val pair of headers
struct header {
    std::string type;
    std::string val;
    std::string ver;
};

//container for data in http request
struct http_req {
    struct header main_req; //Type = http version, val = status code
    struct header headers[16];
};

//container for data in http response
struct http_resp {
    struct header main_resp; //type = http version, val = response code
    //required headers
    struct header content_type;
    struct header location;
    struct header content_length;

    //other headers
    struct header headers[16];
    std::string response_str;
};

//construct response from a respone struct and data to be sent alongside
int response(http_resp * r, char * body_buff) {
    int i = 0;
    
    std::string body = body_buff;

    r->content_length.val = std::to_string(body.length());
    
    //any required headers empty?
    if(r->content_length.val.length() == 0 || r->content_type.val.length() == 0 || r->location.val.length() == 0) {
        std::cout <<"\n not all required headers have values\n" << std::endl;
        return (false);
    }

    //HTTP main header for response
    r->response_str = r->main_resp.type + r->main_resp.val + "\n";

    //adding required headers to response
    r->response_str += "location:" + r->location.val + "\n";
    //r->response_str += "content-type:" + r->content_type.val + "\n"; //REMOVED FOR TESTING
    r->response_str += "content-length:" + r->content_length.val + "\n";

    //add custom headers to response string
    for (int i = 0; r->headers[i].type.length() != 0; i++) {
        r->response_str = r->response_str + r->headers->type + r->headers->val + "\n";
    }

    //finally add response body to response string
    r->response_str += "\n" + body;
    
    return (true);
}

//process and store request data in request struct
int process_req(http_req * r, http_resp * resp, char * req_buff) {
    
    //put buffer in a string object
    std::string req = req_buff; 

    //check if buffer is empty
    if (req.empty()) {
        return (false);
    }
    
    //construct the main http request header
    r->main_req.type = req.substr(0, req.find(" ")); //POST, GET, DELETE etc.
    r->main_req.val = req.substr(req.find("/"), req.find("HTTP") - req.find("/") - 1); //Requested resource
    r->main_req.ver = req.substr(req.find("HTTP"), req.find("\n") - req.find("HTTP")); //HTTP version


    //Creating a new substring of request data, without main http header and body
    req = req.substr(req.find("\n") + 1, req.npos);

    //looping over every header in request data
    for (int i = 0, end = 0; end != true && req.length() > 1; i++) {
        
        r->headers[i].type = req.substr(0, req.find(":"));
        r->headers[i].val = req.substr(req.find(":"), req.find("\n") - req.find(":"));
        std::cout << r->headers[i].type << r->headers[i].val << "\n";

        if (req.find("\n") + 1 < req.find_last_of(req.back())-2 && req.find("\n") != req.npos) {
            //std::cout << req.find("\n") << ":" << req.find_last_of(req.back()) << "\n";
            req = req.substr(req.find("\n") + 1, req.find_last_of(req.back()));
        }

        else {
            end = true;
            //std::cout <<"\n\nend\n\n" << std::endl;
        }
    }

    //std::cout << "request structure created\n";

    std::string directory = "pages\\";
    char body_buff[BUFF_SIZE];
    int res;
    
    //linking urls to files
    links linker;
    linker = link();

    std::cout << r->main_req.val << "\n";

    if (linker.linker.find(r->main_req.val) == linker.linker.end()
        || find_file(linker.linker[r->main_req.val], directory) != true) {
        
        //std::cout << "404 bad request" << std::endl;
        resp->main_resp.type = HTTP_VER;
        resp->main_resp.val = " 404 page not found";

        res = read(linker.linker["404"], directory, body_buff); 
        //resp->content_length.val = std::to_string(resp->body.bytes); //OVERFLOW
        resp->content_type.val = "text/html";
        resp->location.val =  "localhost:3000";

        res = response(resp, body_buff);
    }

    else if (find_file(linker.linker[r->main_req.val], directory) == true){
        //std::cout << "finding file\n";

        resp->main_resp.type = HTTP_VER;
        resp->main_resp.val = " 200 OK";

        if (r->main_req.val.find("jpg") != r->main_req.val.npos) {
            img_read(linker.linker[r->main_req.val], directory, body_buff);
        }

        else {
            res = read(linker.linker[r->main_req.val], directory, body_buff); 
        }
         
        //resp->content_length.val = std::to_string(resp->body.bytes); //OVERFLOW
        resp->content_type.val = "text/html";
        resp->location.val =  "localhost:3000";

        res = response(resp, body_buff);

        //std::cout << "response structure created\n";
    }

    else {
        resp->main_resp.type = HTTP_VER;
        resp->main_resp.val = " 500 server error";

        res = read(linker.linker["500"], directory, body_buff); 
        resp->content_type.val = "text/html";
        resp->location.val =  "localhost:3000";

        response(resp, body_buff);
    }

    strcpy(req_buff, resp->response_str.c_str());

    return (true);
}
