#ifndef SECURITY_H
#define SECURITY_H

#include <string>
#include <iostream>
#include <arpa/inet.h>
#include <sys/socket.h>

class SystemUtils {
public:
	static void log(std::string message, std::string type = "INFO") {
		std::cout << "[" << type << "] " << message << std::endl;
	}
};

class SecurityModule {
public:
	static std::string getClientIP(int socket) {
		sockaddr_in addr; socklen_t addr_size = sizeof(struct sockaddr_in);
		getpeername(socket, (struct sockaddr*)&addr, &addr_size);
		return std::string(inet_ntoa(addr.sin_addr));
	}
	
	static bool isSuspicious(std::string request) {
		if (request.find("' OR '1'='1") != std::string::npos) return true;
		return false;
	}
	
	static void logIncident(std::string ip, std::string request) {
		std::cout << "[ALARM] Angriffsversuch geblockt von IP: " << ip << std::endl;
	}
};
#endif
 
