#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <thread>
#include <unordered_map>
#include <string>
#include <sstream>
#include <filesystem>
#include <vector>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <sys/stat.h>

#include "User_Manager.cpp"
#include "Network.cpp"
#include "Folder_Manager.cpp"
#include "Security.cpp"
#include "Captcha.cpp"
#include "Design_Files.cpp"
#include "Design_Admin.cpp"
#include "Design_Login.cpp"
#include "Design_User.cpp"
#include "Design_Logout.cpp"
#include "Logger.cpp"
#include "Advanced_Features.cpp"
#include "Design_Success.cpp"
#include "Design_Error.cpp" 

namespace fs = std::filesystem;
std::unordered_map<std::string, std::string> onlineUsers;
std::unordered_map<std::string, bool> bannedUsers;

void NetworkHandler::processRequest(int client_socket, UserManager& userDB, std::string ip) {
    if (ip.empty() || ip == "0.0.0.0") {
        try {
            ip = SecurityModule::getClientIP(client_socket);
            if (ip.empty()) ip = "UNKNOWN_IP";
        } catch (...) {
            ip = "UNKNOWN_IP";
        }
    }
    
    char buffer[4096] = {0};
    int valread = read(client_socket, buffer, 4096);
    if (valread <= 0) {
        close(client_socket);
        return;
    }
    
    std::string request(buffer);
    
    // 1. Sicherheitscheck: Angriffe abwehren
    if (request.find("../") != std::string::npos || request.find("/etc/passwd") != std::string::npos) {
        Logger::logSecurity("ATTACK", "Directory Traversal blockiert!", ip);
        
        sendResponse(client_socket, UI::getErrorPage("Sicherheitsverletzung: Directory Traversal Angriff erkannt. Zugriff verweigert.", "403 Forbidden", "Zugriff Verweigert"));
        close(client_socket);
        return;
    }
    
    // 2. GET / ROUTE
    if (request.find("GET / ") != std::string::npos || request.find("GET /index.html") != std::string::npos) {
        std::string loggedInUser = "";
        if (!ip.empty()) loggedInUser = onlineUsers[ip];
        
        if (!loggedInUser.empty()) {
            if (loggedInUser == "admin") {
                sendResponse(client_socket, UI::getAdminDashboard());
            } else {
                sendResponse(client_socket, UI::getUserDashboard(loggedInUser, userDB)); 
            }
        } else {
            std::string code = CaptchaModule::generate(); 
            sendResponse(client_socket, UI::getLoginPage(code)); 
        }
    }
    // 3. GET /logout ROUTE
    else if (request.find("GET /logout") != std::string::npos) {
        if (!ip.empty()) {
            std::string user = onlineUsers[ip];
            if (!user.empty()) {
                Logger::logSystem("AUTH", "User '" + user + "' hat sich abgemeldet.", ip);
                onlineUsers.erase(ip);
            }
        }
        sendResponse(client_socket, UI::getSuccessPage("Du wurdest erfolgreich abgemeldet. Bis bald!", "success"));
    }
    // 4. POST /login ROUTE
    else if (request.find("POST /login") != std::string::npos) {
        std::string targetUser = "";
        size_t userPos = request.find("username=");
        if (userPos != std::string::npos) {
            targetUser = request.substr(userPos + 9);
            size_t endPos = targetUser.find_first_of("&\r\n ");
            if (endPos != std::string::npos) targetUser = targetUser.substr(0, endPos);
        }
        
        bool loginSuccess = false;
        if (targetUser == "admin" && request.find("password=1234") != std::string::npos) {
            loginSuccess = true;
        } else {
            for (const auto& [name, uData] : userDB.users) {
                if (targetUser == name && request.find("password=" + uData.pass) != std::string::npos) {
                    loginSuccess = true;
                    break;
                }
            }
        }
        
        if (!loginSuccess) {
            Logger::logSecurity("WARN", "FEHLGESCHLAGENER LOGIN-VERSUCH fuer User: '" + targetUser + "'", ip);
            sendResponse(client_socket, UI::getErrorPage("Login fehlgeschlagen! Falscher Benutzername oder Passwort.", "error"), "401 Unauthorized");
        } 
        else if (bannedUsers[targetUser]) {
            Logger::logSecurity("BANNED_TRY", "Gebannter User '" + targetUser + "' hat versucht sich einzuloggen.", ip);
            sendResponse(client_socket, UI::getErrorPage("Zugriff verweigert! Dein Konto ist gesperrt.", "error"), "403 Forbidden");
        } 
        else {
            if (!ip.empty()) onlineUsers[ip] = targetUser;
            Logger::logSystem("AUTH", "Erfolgreicher Login: '" + targetUser + "' ist jetzt online.", ip);
            sendResponse(client_socket, UI::getSuccessPage("Willkommen zurück, " + targetUser + "! Du wirst weitergeleitet."));
        }
    }
    // 5. POST /upload ROUTE
    else if (request.find("POST /upload") != std::string::npos) {
        std::string currentUser = onlineUsers[ip];
        
        std::string filename = "uploaded_file.bin";
        size_t namePos = request.find("filename=\"");
        if (namePos != std::string::npos) {
            filename = request.substr(namePos + 10);
            filename = filename.substr(0, filename.find("\""));
        }
        
        if (filename.empty()) {
            sendResponse(client_socket, UI::getErrorPage("Fehler: Kein Dateiname erkannt.", "error"), "400 Bad Request");
            return;
        }
        
        std::string dirPath = "./assets/users/" + currentUser;
        if (!fs::exists(dirPath)) fs::create_directories(dirPath); 
        
        std::string uploadPath = dirPath + "/" + filename;
        std::ofstream outfile(uploadPath, std::ios::binary);
        
        size_t bodyPos = request.find("\r\n\r\n");
        if (bodyPos != std::string::npos) {
            outfile << request.substr(bodyPos + 4);
        }
        outfile.close();
        
        Logger::logSystem("FILE_UPLOAD", "User '" + currentUser + "' hat Datei '" + filename + "' hochgeladen.", ip);
        sendResponse(client_socket, UI::getSuccessPage("Die Datei '" + filename + "' wurde erfolgreich hochgeladen!"));
    }
    // 6. GET /download ROUTE
    else if (request.find("GET /download") != std::string::npos) {
        std::string currentUser = onlineUsers[ip].empty() ? "UNKNOWN" : onlineUsers[ip];
        
        std::string filename = "unbekannt";
        std::string owner = "unknown";
        size_t filePos = request.find("file=");
        if (filePos != std::string::npos) {
            filename = request.substr(filePos + 5);
            size_t endPos = filename.find_first_of("&\r\n ");
            if (endPos != std::string::npos) filename = filename.substr(0, endPos);
            
            size_t ownerPos = request.find("owner=");
            if (ownerPos != std::string::npos) {
                owner = request.substr(ownerPos + 6);
                endPos = owner.find_first_of("&\r\n ");
                if (endPos != std::string::npos) owner = owner.substr(0, endPos);
            }
        }
        
        Logger::logSystem("FILE_DOWNLOAD", "User '" + currentUser + "' lädt die Datei '" + filename + "' herunter.", ip);
        
        std::string fullPath = "./assets/users/" + owner + "/" + filename;
        std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            
            sendResponse(client_socket, UI::getErrorPage("Die angeforderte Datei '" + filename + "' wurde nicht gefunden.", "404 Not Found", "Datei nicht gefunden"));
            return;
        }
        
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            
            sendResponse(client_socket, UI::getErrorPage("Ein interner Fehler ist beim Lesen der Datei aufgetreten.", "500 Internal Server Error", "Interner Fehler"));
            return;
        }
        file.close();
        
        std::string responseHeaders = "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/octet-stream\r\n"
        "Content-Disposition: attachment; filename=\"" + filename + "\"\r\n"
        "Content-Length: " + std::to_string(size) + "\r\n"
        "Connection: close\r\n\r\n";
        
        send(client_socket, responseHeaders.c_str(), responseHeaders.size(), 0);
        send(client_socket, buffer.data(), size, 0);
    }
    // 7. GET /admin/api/online ROUTE
    else if (request.find("GET /admin/api/online") != std::string::npos) {
        std::string htmlList = "<table class='status-table'>";
        for (auto const& [username, uData] : userDB.users) {
            if (username.empty()) continue; 
            bool isOnline = false;
            for (const auto& pair : onlineUsers) {
                if (pair.second == username) { isOnline = true; break; }
            }
            std::string statusText = "";
            if (bannedUsers[username]) {
                statusText = "<span style='color:#ffaa00;'>🔒 GEBANNT</span>";
            } else if (isOnline) {
                statusText = "<span style='color:#00ff00;'>🟢 ONLINE</span>";
            } else {
                statusText = "<span style='color:#ff4a4a;'>🔴 OFFLINE</span>";
            }
            if (uData.warnings > 0) {
                statusText += " <span style='color:yellow;'>(" + std::to_string(uData.warnings) + "x Warn)</span>";
            }
            htmlList += "<tr><td><strong>" + username + "</strong></td><td>" + statusText + "</td></tr>";
        }
        htmlList += "</table>";
        sendResponse(client_socket, htmlList);
    }
    // 8. GET /admin/api/files ROUTE
    else if (request.find("GET /admin/api/files") != std::string::npos) {
        sendResponse(client_socket, FileServer::generateAdminFileListHTML());
    }
    // 9. POST /admin/ban ROUTE
    else if (request.find("POST /admin/ban") != std::string::npos) {
        size_t pos = request.find("username=");
        if (pos != std::string::npos) {
            std::string targetUser = request.substr(pos + 9);
            size_t endPos = targetUser.find_first_of("&\r\n ");
            if (endPos != std::string::npos) targetUser = targetUser.substr(0, endPos);
            if (!targetUser.empty() && targetUser != "admin") {
                bannedUsers[targetUser] = true;
                Logger::logSystem("ADMIN_ACTION", "User '" + targetUser + "' wurde vom Admin GEBANNT!", ip);
                for (auto it = onlineUsers.begin(); it != onlineUsers.end(); ) {
                    if (it->second == targetUser) it = onlineUsers.erase(it);
                    else ++it;
                }
                sendResponse(client_socket, UI::getSuccessPage("Der Benutzer '" + targetUser + "' wurde erfolgreich GEBANNT!"));
                return;
            }
        }
        sendResponse(client_socket, UI::getErrorPage("Fehler: Ungültiger Benutzername.", "error"), "400 Bad Request");
    }
    // 10. POST /admin/unban ROUTE
    else if (request.find("POST /admin/unban") != std::string::npos) {
        size_t pos = request.find("username=");
        if (pos != std::string::npos) {
            std::string targetUser = request.substr(pos + 9);
            size_t endPos = targetUser.find_first_of("&\r\n ");
            if (endPos != std::string::npos) targetUser = targetUser.substr(0, endPos);
            if (!targetUser.empty() && targetUser != "admin") {
                bannedUsers[targetUser] = false;
                Logger::logSystem("ADMIN_ACTION", "User '" + targetUser + "' wurde vom Admin ENTBANNT.", ip);
                sendResponse(client_socket, UI::getSuccessPage("Der Benutzer '" + targetUser + "' wurde erfolgreich ENTBANNT!"));
                return;
            }
        }
        sendResponse(client_socket, UI::getErrorPage("Fehler: Ungültiger Benutzername.", "error"), "400 Bad Request");
    }
    // 11. Create-User Route
    else if (request.find("POST /admin/create") != std::string::npos) {
        std::string newUsername = "", newPass = "";
        size_t uPos = request.find("username=");
        size_t pPos = request.find("password=");
        if (uPos != std::string::npos && pPos != std::string::npos) {
            newUsername = request.substr(uPos + 9, request.find('&', uPos) - (uPos + 9));
            newPass = request.substr(pPos + 9);
            size_t endP = newPass.find_first_of(" \r\n");
            if (endP != std::string::npos) newPass = newPass.substr(0, endP);
            if (newUsername.empty() || newPass.empty()) {
                sendResponse(client_socket, UI::getErrorPage("Fehler: Benutzername und Passwort dürfen nicht leer sein.", "error"), "400 Bad Request");
                return;
            }
            userDB.users[newUsername] = {newPass, false, userDB.getCurrentDateTime(), 0};
            userDB.saveUsersToFile(); 
            std::string userFolderPath = "./assets/users/" + newUsername;
            struct stat info;
            if (stat(userFolderPath.c_str(), &info) != 0) {
                mkdir(userFolderPath.c_str(), 0755);
            }
            Logger::logSystem("ADMIN_ACTION", "Admin hat User '" + newUsername + "' erstellt.", ip);
            sendResponse(client_socket, UI::getSuccessPage("Der Benutzer '" + newUsername + "' wurde erfolgreich erstellt!"));
            return;
        }
    }
    // 12. Abmahnung-User + Zähler Route
    else if (request.find("POST /admin/warn") != std::string::npos) {
        size_t pos = request.find("username=");
        if (pos != std::string::npos) {
            std::string targetUser = request.substr(pos + 9);
            size_t endPos = targetUser.find_first_of("&\r\n ");
            if (endPos != std::string::npos) targetUser = targetUser.substr(0, endPos);
            if (userDB.users.count(targetUser)) {
                userDB.users[targetUser].warnings++; 
                userDB.saveUsersToFile(); 
                sendResponse(client_socket, UI::getSuccessPage("Der Benutzer '" + targetUser + "' wurde erfolgreich ABGEMAHNT!"));
                return;
            }
        }
        sendResponse(client_socket, UI::getErrorPage("Fehler: Benutzer nicht gefunden.", "error"), "404 Not Found");
    }
    // 13. POST /admin/delete Route
    else if (request.find("POST /admin/delete") != std::string::npos) {
        size_t pos = request.find("username=");
        if (pos != std::string::npos) {
            std::string targetUser = request.substr(pos + 9);
            size_t endPos = targetUser.find_first_of("&\r\n ");
            if (endPos != std::string::npos) targetUser = targetUser.substr(0, endPos);
            if (!targetUser.empty() && targetUser != "admin" && userDB.users.count(targetUser)) {
                std::string userFolderPath = "./assets/users/" + targetUser;
                std::string fileList = "Gelöschte Dateien: ";
                if (fs::exists(userFolderPath)) {
                    for (const auto& entry : fs::directory_iterator(userFolderPath)) {
                        fileList += entry.path().filename().string() + ", ";
                    }
                }
                std::ofstream logFile("deleted_users.log", std::ios::app);
                if (logFile.is_open()) {
                    logFile << "[" << userDB.getCurrentDateTime() << "] User gelöscht: " << targetUser 
                    << " | " << fileList << std::endl;
                    logFile.close();
                }
                userDB.users.erase(targetUser);
                userDB.saveUsersToFile();
                if (fs::exists(userFolderPath)) {
                    fs::remove_all(userFolderPath);
                }
                Logger::logSystem("ADMIN_ACTION", "User '" + targetUser + "' inkl. Dateien gelöscht.", ip);
                sendResponse(client_socket, UI::getSuccessPage("Der Benutzer '" + targetUser + "' wurde erfolgreich gelöscht!"));
                return;
            }
        }
        sendResponse(client_socket, UI::getErrorPage("Fehler: Benutzer nicht gefunden oder ungültige Aktion.", "error"), "404 Not Found");
    }
    // 14. POST /change-password ROUTE
    else if (request.find("POST /change-password") != std::string::npos) {
        std::string currentUser = onlineUsers[ip];
        if (currentUser.empty()) {
            sendResponse(client_socket, UI::getErrorPage("Fehler: Nicht eingeloggt.", "error"), "401 Unauthorized");
            return;
        }
        std::string oldPass = "", newPass = "";
        size_t oldPos = request.find("old_password=");
        if (oldPos != std::string::npos) {
            oldPass = request.substr(oldPos + 13);
            size_t endPos = oldPass.find_first_of("&\r\n ");
            if (endPos != std::string::npos) oldPass = oldPass.substr(0, endPos);
        }
        size_t newPos = request.find("new_password=");
        if (newPos != std::string::npos) {
            newPass = request.substr(newPos + 13);
            size_t endPos = newPass.find_first_of("&\r\n ");
            if (endPos != std::string::npos) newPass = newPass.substr(0, endPos);
        }
        if (oldPass.empty() || newPass.empty()) {
            Logger::logSecurity("WARN", "Leere Passwortfelder bei Änderung für User '" + currentUser + "'.", ip);
            sendResponse(client_socket, UI::getErrorPage("Fehler: Bitte beide Felder ausfüllen!", "error"), "400 Bad Request");
            return;
        }
        if (userDB.users.find(currentUser) == userDB.users.end()) {
            sendResponse(client_socket, UI::getErrorPage("Fehler: User nicht gefunden.", "error"), "404 Not Found");
            return;
        }
        if (userDB.users[currentUser].pass != oldPass) {
            Logger::logSecurity("WARN", "Falsches altes Passwort für User '" + currentUser + "'.", ip);
            sendResponse(client_socket, UI::getErrorPage("Fehler: Altes Passwort ist falsch!", "error"), "401 Unauthorized");
            return;
        }
        userDB.users[currentUser].pass = newPass;
        userDB.saveUsersToFile();
        Logger::logSystem("SECURITY", "User '" + currentUser + "' hat sein Passwort erfolgreich geändert.", ip);
        sendResponse(client_socket, UI::getSuccessPage("Dein Passwort wurde erfolgreich geändert!"));
    }
    // 15. POST /send-file ROUTE
    else if (request.find("POST /send-file") != std::string::npos) {
        std::string currentUser = onlineUsers[ip];
        if (currentUser.empty()) {
            sendResponse(client_socket, UI::getErrorPage("Fehler: Nicht eingeloggt.", "error"), "401 Unauthorized");
            return;
        }
        std::string targetUser = "", filename = "";
        size_t tPos = request.find("target_user=");
        if (tPos != std::string::npos) {
            targetUser = request.substr(tPos + 12);
            size_t endPos = targetUser.find_first_of("&\r\n ");
            if (endPos != std::string::npos) targetUser = targetUser.substr(0, endPos);
        }
        size_t fPos = request.find("filename=");
        if (fPos != std::string::npos) {
            filename = request.substr(fPos + 9);
            size_t endPos = filename.find_first_of("&\r\n ");
            if (endPos != std::string::npos) filename = filename.substr(0, endPos);
        }
        if (targetUser.empty() || filename.empty()) {
            sendResponse(client_socket, UI::getErrorPage("Fehler: Bitte User und Datei auswählen.", "error"), "400 Bad Request");
            return;
        }
        if (targetUser == currentUser) {
            sendResponse(client_socket, UI::getErrorPage("Fehler: Du kannst dir nicht selbst eine Datei senden.", "error"), "400 Bad Request");
            return;
        }
        if (userDB.users.find(targetUser) == userDB.users.end()) {
            sendResponse(client_socket, UI::getErrorPage("Fehler: Ziel-User existiert nicht.", "error"), "404 Not Found");
            return;
        }
        if (AdvancedFeatures::sendFileToUser(currentUser, targetUser, filename)) {
            Logger::logSystem("FILE_SHARE", "User '" + currentUser + "' hat Datei '" + filename + "' an '" + targetUser + "' gesendet.", ip);
            sendResponse(client_socket, UI::getSuccessPage("Die Datei '" + filename + "' wurde erfolgreich an '" + targetUser + "' gesendet!"));
        } else {
            sendResponse(client_socket, UI::getErrorPage("Fehler: Datei nicht gefunden oder Senden fehlgeschlagen.", "error"), "500 Internal Server Error");
        }
    }
    // 16. GET /download_inbox ROUTE
    else if (request.find("GET /download_inbox") != std::string::npos) {
        std::string currentUser = onlineUsers[ip].empty() ? "UNKNOWN" : onlineUsers[ip];
        std::string filename = "unbekannt";
        std::string owner = "unknown";
        size_t filePos = request.find("file=");
        if (filePos != std::string::npos) {
            filename = request.substr(filePos + 5);
            size_t endPos = filename.find_first_of("&\r\n ");
            if (endPos != std::string::npos) filename = filename.substr(0, endPos);
            size_t ownerPos = request.find("owner=");
            if (ownerPos != std::string::npos) {
                owner = request.substr(ownerPos + 6);
                endPos = owner.find_first_of("&\r\n ");
                if (endPos != std::string::npos) owner = owner.substr(0, endPos);
            }
        }
        if (owner != currentUser) {
            
            sendResponse(client_socket, UI::getErrorPage("Du hast keine Berechtigung, diese Datei herunterzuladen.", "403 Forbidden", "Zugriff Verweigert"));
            return;
        }
        std::string fullPath = "./assets/inbox/" + owner + "/" + filename;
        std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            
            sendResponse(client_socket, UI::getErrorPage("Die angeforderte Datei wurde nicht gefunden.", "404 Not Found", "Datei nicht gefunden"));
            return;
        }
        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);
        std::vector<char> buffer(size);
        if (!file.read(buffer.data(), size)) {
            
            sendResponse(client_socket, UI::getErrorPage("Ein interner Fehler ist beim Lesen der Datei aufgetreten.", "500 Internal Server Error", "Interner Fehler"));
            return;
        }
        file.close();
        std::string responseHeaders = "HTTP/1.1 200 OK\r\n"
        "Content-Type: application/octet-stream\r\n"
        "Content-Disposition: attachment; filename=\"" + filename + "\"\r\n"
        "Content-Length: " + std::to_string(size) + "\r\n"
        "Connection: close\r\n\r\n";
        send(client_socket, responseHeaders.c_str(), responseHeaders.size(), 0);
        send(client_socket, buffer.data(), size, 0);
    }
    // 17. FALLBACK: Unbekannte Route
    else {
        
        sendResponse(client_socket, UI::getErrorPage("Die angeforderte Seite oder Aktion existiert nicht.", "404 Not Found", "Seite nicht gefunden"));
    }
} 

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) return 1;
    
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) return 1;
    if (listen(server_fd, 10) < 0) return 1;
    
    UserManager userDB;
    FolderManager::initBaseStructure();
    
    Logger::logSystem("STARTUP", "### CloudServer erfolgreich gestartet ###");
    while(true) {
        sockaddr_in client_addr;
        socklen_t addr_len = sizeof(client_addr);
        int client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket >= 0) {
            std::thread([client_socket, &userDB]() {
                NetworkHandler::processRequest(client_socket, userDB);
            }).detach();
        }
    }
    return 0;
}
