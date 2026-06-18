#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <string>
#include <unordered_map>
#include <sys/socket.h>
#include <unistd.h>

extern std::unordered_map<std::string, std::string> onlineUsers;
extern std::unordered_map<std::string, bool> bannedUsers;

class UserManager;

class NetworkHandler {
private:
    static void sendResponse(int client_socket, const std::string& content, const std::string& status = "200 OK", const std::string& contentType = "text/html") {
        std::string response = "HTTP/1.1 " + status + "\r\n"
        + "Content-Type: " + contentType + "\r\n"
        + "Content-Length: " + std::to_string(content.size()) + "\r\n"
        + "Connection: close\r\n\r\n"
        + content;
        send(client_socket, response.c_str(), response.size(), 0);
    }
    
public:
    static void processRequest(int client_socket, UserManager& userDB, std::string ip = "");
};

#endif
