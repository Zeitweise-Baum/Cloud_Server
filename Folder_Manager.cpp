#ifndef FOLDER_MANAGER_H
#define FOLDER_MANAGER_H

#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

class FolderManager {
private:
	inline static const std::string BASE_PATH = "./assets/";  // ✅ Konsistent
	
public:
	static void initBaseStructure() {
		mkdir(BASE_PATH.c_str(), 0777);
		mkdir((BASE_PATH + "admin").c_str(), 0777);
		mkdir((BASE_PATH + "user").c_str(), 0777);
	}
	
	static bool createUserFolder(const std::string& username) {
		std::string userPath = BASE_PATH + username;
		if (mkdir(userPath.c_str(), 0777) == 0) {
			std::cout << "[SYSTEM] Ordnerstruktur fuer User '" << username << "' erfolgreich angelegt." << std::endl;
			return true;
		}
		return false;
	}
	
	static std::string getUserPath(const std::string& username) {
		return BASE_PATH + username + "/";
	}
};

#endif
