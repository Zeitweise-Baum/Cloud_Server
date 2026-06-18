#ifndef ADVANCED_FEATURES_H
#define ADVANCED_FEATURES_H

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <ctime>
#include <iomanip>
#include "User_Manager.cpp"

namespace fs = std::filesystem;

class AdvancedFeatures {
private:
	static std::string getTimestamp() {
		auto now = std::chrono::system_clock::now();
		auto in_time_t = std::chrono::system_clock::to_time_t(now);
		std::stringstream ss;
		ss << std::put_time(std::localtime(&in_time_t), "%d.%m.%Y %H:%M");
		return ss.str();
	}
	
public:
	// ✅ HILFSFUNKTIONEN JETZT PUBLIC!
	static std::string generateUserSelectList(UserManager& userDB, const std::string& currentUser) {
		std::string html = "";
		for (auto const& [name, uData] : userDB.users) {
			if (name != currentUser) {
				html += "<option value='" + name + "'>" + name + "</option>\n";
			}
		}
		return html;
	}
	
	static std::string generateFileSelectList(const std::string& username) {
		std::string userPath = "./assets/users/" + username + "/";
		std::string html = "";
		
		if (fs::exists(userPath)) {
			for (const auto& entry : fs::directory_iterator(userPath)) {
				if (entry.is_regular_file()) {
					html += "<option value='" + entry.path().filename().string() + "'>" + entry.path().filename().string() + "</option>\n";
				}
			}
		}
		return html;
	}
	
	// 1. Statistiken für den User abrufen
	static std::string getUserStats(const std::string& username) {
		std::string userPath = "./assets/users/" + username + "/";
		int fileCount = 0;
		std::string lastUpload = "Noch keine Dateien";
		
		if (fs::exists(userPath)) {
			for (const auto& entry : fs::directory_iterator(userPath)) {
				if (entry.is_regular_file()) {
					fileCount++;
					lastUpload = entry.path().filename().string();
				}
			}
		}
		
		std::string statsHTML = "<div style='background:rgba(0,0,0,0.4); padding:15px; border-radius:5px; margin-top:15px; border:1px solid #4ecca3;'>";
		statsHTML += "<h3 style='margin-top:0; color:#4ecca3;'>📊 Benutzerstatistik</h3>";
		statsHTML += "<p style='margin:5px 0;'><strong>📁 Hochgeladene Dateien:</strong> <span style='color:#fff;'>" + std::to_string(fileCount) + "</span></p>";
		statsHTML += "<p style='margin:5px 0;'><strong>🕒 Zuletzt hochgeladen:</strong> <span style='color:#aaa;'>" + lastUpload + "</span></p>";
		statsHTML += "</div>";
		
		return statsHTML;
	}
	
	// 2. Inbox für empfangene Dateien anzeigen
	static std::string getInboxHTML(const std::string& username) {
		std::string inboxPath = "./assets/inbox/" + username + "/";
		
		if (!fs::exists("./assets/inbox")) fs::create_directories("./assets/inbox");
		if (!fs::exists(inboxPath)) fs::create_directories(inboxPath);
		
		std::string html = "<div style='margin-top:20px;'>";
		html += "<h3 style='color:#4ecca3; border-bottom:1px solid #4ecca3; padding-bottom:5px;'>📥 Eingehende Dateien (Inbox)</h3>";
		
		bool hasFiles = false;
		if (fs::exists(inboxPath)) {
			html += "<ul style='list-style:none; padding:0;'>";
			for (const auto& entry : fs::directory_iterator(inboxPath)) {
				if (entry.is_regular_file()) {
					hasFiles = true;
					std::string fName = entry.path().filename().string();
					html += "<li style='background:rgba(0,0,0,0.3); padding:8px; margin-bottom:5px; border-left:3px solid #4ecca3;'>";
					html += "📄 " + fName + " ";
					html += "<a href='/download_inbox?file=" + fName + "&owner=" + username + "' style='color:#4ecca3; text-decoration:none; margin-left:10px; font-weight:bold;'>[Herunterladen]</a>";
					html += "</li>";
				}
			}
			html += "</ul>";
		}
		
		if (!hasFiles) {
			html += "<p style='color:#888; font-style:italic;'>Deine Inbox ist leer.</p>";
		}
		
		html += "</div>";
		return html;
	}
	
	// 3. Datei an anderen User senden
	static bool sendFileToUser(const std::string& sender, const std::string& receiver, const std::string& filename) {
		std::string sourcePath = "./assets/users/" + sender + "/" + filename;
		std::string destPath = "./assets/inbox/" + receiver + "/" + filename;
		
		if (!fs::exists(sourcePath)) return false;
		
		std::string receiverInbox = "./assets/inbox/" + receiver;
		if (!fs::exists(receiverInbox)) fs::create_directories(receiverInbox);
		
		try {
			fs::copy_file(sourcePath, destPath, fs::copy_options::overwrite_existing);
			return true;
		} catch (...) {
			return false;
		}
	}
};

#endif
