#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <chrono>
#include <iomanip>

struct UserData {
    std::string pass;
    bool isAdmin;
    std::string regTime;
    int warnings = 0;
};

class UserManager {
public:
    std::unordered_map<std::string, UserData> users;
    
    UserManager() {
        loadUsersFromFile();
    }
    
    std::string getCurrentDateTime() {
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&in_time_t), "%d.%m.%Y %H:%M");
        return ss.str();
    }
    
    void saveUsersToFile() {
        std::ofstream file("users.txt");
        if (file.is_open()) {
            for (auto const& [name, d] : users) {
                file << name << ";" << d.pass << ";" << (d.isAdmin ? "1" : "0") 
                << ";" << d.regTime << ";" << d.warnings << "\n";
            }
            file.close();
        }
    }
    
    void loadUsersFromFile() {
        std::ifstream file("users.txt");
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::string item, name, pass, adminStr, time, warnStr;
            
            std::getline(ss, name, ';');
            std::getline(ss, pass, ';');
            std::getline(ss, adminStr, ';');
            std::getline(ss, time, ';');
            std::getline(ss, warnStr, ';');
            
            if (!name.empty()) {
                users[name] = {pass, (adminStr == "1"), time, std::stoi(warnStr.empty() ? "0" : warnStr)};
            }
        }
    }
};

#endif
