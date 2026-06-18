#ifndef LOGGER_H
#define LOGGER_H

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <netdb.h>
#include <arpa/inet.h>

class Logger {
private:
    static std::string resolveHost(const std::string& ip) {
        if (ip == "127.0.0.1" || ip == "localhost") return "LOKALER_HOST (Loopback)";
        
        struct sockaddr_in sa;
        char host[1024];
        
        sa.sin_family = AF_INET;
        if (inet_pton(AF_INET, ip.c_str(), &sa.sin_addr) == 1) {
            if (getnameinfo((struct sockaddr*)&sa, sizeof(sa), host, sizeof(host), NULL, 0, NI_NAMEREQD) == 0) {
                return std::string(host);
            }
        }
        return "Unbekannter Provider / Direkt-IP";
    }
    
    static std::string getTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << "[" << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %H:%M:%S") << "] ";
        return ss.str();
    }
    
    static void writeToFile(const std::string& fileName, const std::string& logLine) {
        std::ofstream logFile(fileName, std::ios::app);
        if (logFile.is_open()) {
            logFile << logLine << "\n";
            logFile.close();
        } else {
            std::cerr << "\033[1;41;37m[KRITISCHER FEHLER]\033[0m Log-Datei " << fileName << " konnte nicht geschrieben werden!" << std::endl;
        }
    }
    
public:
    static void logSystem(const std::string& category, const std::string& message, const std::string& ip = "UNKNOWN") {
        std::string logLine = getTimestamp() + "[" + category + "] [IP: " + ip + "] " + message;
        
        if (category == "ADMIN_ACTION" || category == "USER_REG") {
            std::cout << "\033[1;33m" << logLine << "\033[0m" << std::endl;
        } else {
            std::cout << "\033[1;32m" << logLine << "\033[0m" << std::endl;
        }
        
        writeToFile("server_system.log", logLine);
    }
    
    static void logSecurity(const std::string& category, const std::string& message, const std::string& ip = "UNKNOWN") {
        std::string hostInfo = resolveHost(ip);
        std::string logLine = getTimestamp() + "[SECURITY-" + category + "] [IP: " + ip + " (" + hostInfo + ")] " + message;
        
        std::cout << "\033[1;31m" << logLine << "\033[0m" << std::endl;
        
        writeToFile("server_security.log", logLine);
    }
    
    static void logError(const std::string& category, const std::string& message) {
        std::string logLine = getTimestamp() + "[ERROR-" + category + "] " + message;
        
        std::cout << "\033[1;41;37m" << logLine << "\033[0m" << std::endl;
        
        writeToFile("server_error.log", logLine);
    }
};

#endif
