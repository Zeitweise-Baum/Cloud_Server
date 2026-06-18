#ifndef DESIGN_FILES_H
#define DESIGN_FILES_H

#include <string>
#include <sys/socket.h>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <vector>

class FileServer {
public:
	// 1. REGULÄRE USER-DATEILISTE
	static std::string generateFileListHTML(const std::string& username) {
		std::string html = "<div style='font-family:monospace; color:#fff; max-height:400px; overflow-y:auto;'>";
		std::string basePath = "./assets/users/" + username + "/";  // ✅ KORREKT
		
		DIR* dir = opendir(basePath.c_str());
		if (!dir) {
			return "<p style='color:#666; font-style:italic;'>Noch keine Dateien hochgeladen.</p>";
		}
		
		struct dirent* fileEntry;
		bool hatDateien = false;
		
		html += "<ul style='list-style:none; padding-left:0; margin-top:5px;'>";
		while ((fileEntry = readdir(dir)) != nullptr) {
			if (fileEntry->d_name[0] != '.') {
				hatDateien = true;
				std::string fName(fileEntry->d_name);
				html += "<li style='margin-bottom:8px; color:#ddd; border-bottom: 1px dashed #222; padding-bottom:5px;'>📄 " + fName + " "
				+ "<a href='/download?file=" + fName + "&owner=" + username + "' "
				+ "style='color:#4ecca3; margin-left:10px; text-decoration:none; font-weight:bold;'>[Herunterladen]</a></li>";
			}
		}
		html += "</ul>";
		closedir(dir);
		
		if (!hatDateien) {
			html += "<p style='color:#666; font-style:italic;'>Ordner ist leer</p>";
		}
		
		html += "</div>";
		return html;
	}
	
	// 2. ADMIN-DATEILISTE (Scannt das gesamte Assets-Verzeichnis für ALLE User)
	static std::string generateAdminFileListHTML() {
		std::string html = "<div style='font-family:monospace; color:#fff; max-height:400px; overflow-y:auto;'>";
		std::string basePath = "./assets/users/";
		
		DIR* baseDir = opendir(basePath.c_str());
		if (!baseDir) {
			return "<p style='color:#ff5e5e;'>Fehler: assets-Verzeichnis nicht gefunden.</p>";
		}
		
		struct dirent* userEntry;
		bool hatOrdner = false;
		
		while ((userEntry = readdir(baseDir)) != nullptr) {
			// Punkt-Ordner überspringen
			if (userEntry->d_name[0] == '.') continue;
			
			// ROBUSTE Verzeichnis-Prüfung (für DT_UNKNOWN Fälle)
			bool isDir = false;
			if (userEntry->d_type == DT_DIR) {
				isDir = true;
			} else if (userEntry->d_type == DT_UNKNOWN) {
				// Falls Typ unbekannt ist, stat() aufrufen
				struct stat st;
				std::string pathToCheck = basePath + std::string(userEntry->d_name);
				if (stat(pathToCheck.c_str(), &st) == 0 && S_ISDIR(st.st_mode)) {
					isDir = true;
				}
			}
			
			if (isDir) {
				hatOrdner = true;
				std::string currentFolder(userEntry->d_name);
				
				html += "<div style='margin-bottom:15px; border-bottom:1px dashed #333; padding-bottom:10px;'>";
				html += "<span style='color:#4ecca3; font-weight:bold;'>📁 Ordner: " + currentFolder + "</span>";
				html += "<ul style='list-style:none; padding-left:20px; margin-top:5px;'>";
				
				std::string userPath = basePath + currentFolder + "/";
				DIR* dir = opendir(userPath.c_str());
				if (dir) {
					struct dirent* fileEntry;
					bool hatDateien = false;
					while ((fileEntry = readdir(dir)) != nullptr) {
						if (fileEntry->d_name[0] != '.') {
							hatDateien = true;
							std::string fName(fileEntry->d_name);
							html += "<li style='margin-bottom:4px; color:#ddd;'>📄 " + fName + " "
							+ "<a href='/download?file=" + fName + "&owner=" + currentFolder + "' "
							+ "style='color:#ffb703; margin-left:10px; text-decoration:none;'>[Herunterladen]</a></li>";
						}
					}
					if (!hatDateien) {
						html += "<li style='color:#666; font-style:italic;'>Ordner ist leer</li>";
					}
					closedir(dir);
				}
				html += "</ul></div>";
			}
		}
		closedir(baseDir);
		
		if (!hatOrdner) {
			html += "<p style='color:#888;'>Keine Benutzerverzeichnisse vorhanden.</p>";
		}
		
		html += "</div>";
		return html;
	}
	
	// 3. STATISCHE DATEIEN SERVIEREN
	static void serveStaticFile(int socket, const std::string& relativePath, const std::string& contentType) {
		std::string fullPath = "./assets/users" + relativePath;  // ✅ KORREKT
		
		std::ifstream file(fullPath, std::ios::binary | std::ios::ate);
		if (!file.is_open()) {
			std::string notFound = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\nConnection: close\r\n\r\n";
			send(socket, notFound.c_str(), notFound.size(), 0);
			return;
		}
		
		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);
		
		std::vector<char> buffer(size);
		if (!file.read(buffer.data(), size)) {
			return;
		}
		file.close();
		
		std::string fileName = relativePath;
		size_t lastSlash = relativePath.find_last_of("/");
		if (lastSlash != std::string::npos) {
			fileName = relativePath.substr(lastSlash + 1);
		}
		
		std::string responseHeaders = "HTTP/1.1 200 OK\r\n"
		"Content-Type: " + contentType + "\r\n"
		"Content-Disposition: attachment; filename=\"" + fileName + "\"\r\n"
		"Content-Length: " + std::to_string(size) + "\r\n"
		"Connection: close\r\n\r\n";
		
		send(socket, responseHeaders.c_str(), responseHeaders.size(), 0);
		send(socket, buffer.data(), size, 0);
	}
};

#endif
