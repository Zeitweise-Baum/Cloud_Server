#ifndef DESIGN_USER_H
#define DESIGN_USER_H

#include <string>
#include "User_Manager.cpp"
#include "Design_Files.cpp"
#include "Advanced_Features.cpp" // ✅ WICHTIG: Dieser Include muss hier stehen!

namespace UI {
    std::string getUserDashboard(std::string username, UserManager& userDB) {
        // 1. Warnungen-Tabelle
        std::string warnTable = "<h2>Regelverstöße im System</h2>";
        warnTable += "<table class='status-table' style='width: 100%; border-collapse: collapse;'>";
        warnTable += "<tr><th style='text-align:left; border-bottom:1px solid #4ecca3;'>User</th><th style='text-align:left; border-bottom:1px solid #4ecca3;'>Warnungen</th></tr>";
        
        for (auto const& [name, uData] : userDB.users) {
            if (uData.warnings > 0) {
                warnTable += "<tr><td style='padding:5px 0;'>" + name + "</td><td style='color:yellow;'>" 
                + std::to_string(uData.warnings) + "x verwarnt</td></tr>";
            }
        }
        warnTable += "</table><br>";
        
        // 2. Dateiliste (Eigene Dateien)
        std::string fileListHTML = FileServer::generateFileListHTML(username);
        
        // 3. Statistik (Neu!)
        std::string statsHTML = AdvancedFeatures::getUserStats(username);
        
        // 4. Inbox (Neu!)
        std::string inboxHTML = AdvancedFeatures::getInboxHTML(username);
        
        return R"html(
            <!DOCTYPE html>
            <html lang="de">
            <head>
                <meta charset="UTF-8">
                <title>User Cloud Terminal</title>
                <style>
                    /* ... (Dein CSS bleibt unverändert) ... */
                    body { margin: 0; padding: 0; font-family: 'Courier New', monospace; background-color: #0a0f1d; color: #4ecca3; display: flex; justify-content: center; align-items: center; min-height: 100vh; }
                    .container { width: 85%; max-width: 900px; background: rgba(10, 20, 40, 0.75); border: 1px solid #4ecca3; padding: 30px; border-radius: 15px; box-shadow: 0 0 25px rgba(78, 204, 163, 0.4); }
                    h1, h2, h3 { text-transform: uppercase; letter-spacing: 2px; border-bottom: 1px solid #4ecca3; padding-bottom: 10px; }
                    .file-system { margin-top: 20px; background: rgba(0,0,0,0.5); padding: 20px; border-radius: 5px; height: 180px; border: 1px dashed #4ecca3; overflow-y: auto; }
                    .upload-area { margin-top: 20px; display: flex; align-items: center; gap: 15px; background: rgba(0,0,0,0.3); padding: 15px; border: 1px solid #4ecca3; border-radius: 5px; }
                    input[type="file"] { display: none; }
                    .custom-file-upload { border: 1px solid #4ecca3; display: inline-block; padding: 10px 20px; cursor: pointer; font-weight: bold; font-family: inherit; color: #4ecca3; background: transparent; transition: all 0.3s; text-transform: uppercase; }
                    .custom-file-upload:hover { background: #4ecca3; color: #0a0f1d; box-shadow: 0 0 10px #4ecca3; }
                    .btn-upload { background: transparent; border: 1px solid #4ecca3; color: #4ecca3; padding: 10px 25px; border-radius: 5px; cursor: pointer; font-weight: bold; font-family: inherit; text-transform: uppercase; transition: all 0.3s; }
                    .btn-upload:hover { background: #4ecca3; color: #0a0f1d; box-shadow: 0 0 10px #4ecca3; }
                    .file-name-display { flex-grow: 1; color: #888; font-style: italic; white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
                    .password-section { margin-top: 25px; background: rgba(0,0,0,0.3); padding: 20px; border: 1px solid #4ecca3; border-radius: 5px; }
                    .password-section h3 { margin-top: 0; color: #4ecca3; border-bottom: 1px solid #4ecca3; padding-bottom: 5px; font-size: 16px; }
                    .pwd-input { width: 100%; background: #000; border: 1px solid #4ecca3; color: #fff; padding: 8px; margin-bottom: 10px; font-family: inherit; box-sizing: border-box; outline: none; }
                    .pwd-input:focus { border-color: #fff; box-shadow: 0 0 5px #4ecca3; }
                    .btn-pwd { background: transparent; border: 1px solid #4ecca3; color: #4ecca3; padding: 8px 15px; cursor: pointer; font-weight: bold; font-family: inherit; text-transform: uppercase; transition: all 0.3s; width: 100%; }
                    .btn-pwd:hover { background: #4ecca3; color: #0a0f1d; box-shadow: 0 0 10px #4ecca3; }
                    .btn-danger { border-color: #ff4a4a; color: #ff4a4a; }
                    .btn-danger:hover { background: #ff4a4a; color: #fff; }
                    </style>
                    </head>
                    <body>
                    <div class="container">
                    <h1>Cloud Terminal v1.0.0</h1>
                    <h2>Willkommen, <strong>)html" + username + R"html(</strong></h2>
                    
                    )html" + warnTable + R"html(
                        
                        <!-- Statistik -->
                        )html" + statsHTML + R"html(
                            
                            <!-- Eigene Dateien -->
                            <div class="file-system">
                            )html" + fileListHTML + R"html(
                                </div>
                                
                                <!-- Upload Bereich -->
                                <div class="upload-area">
                                <label for="file-upload" class="custom-file-upload">📂 Durchsuchen</label>
                                <input id="file-upload" type="file" name="file" required onchange="updateFileName(this)"/>
                                <span id="file-name" class="file-name-display">Keine Datei ausgewählt</span>
                                <button type="submit" form="upload-form" class="btn-upload">Hochladen</button>
                                </div>
                                <form id="upload-form" action="/upload" method="POST" enctype="multipart/form-data" style="display: none;"></form>
                                
                                <!-- Datei senden Bereich -->
                                <div class="password-section" style="margin-top: 25px;">
                                <h3>📨 Datei an anderen User senden</h3>
                                <form action="/send-file" method="POST" style="display: flex; flex-direction: column; gap: 10px;">
                                <select name="target_user" class="pwd-input" required>
                                <option value="">-- User auswählen --</option>
                                )html" + AdvancedFeatures::generateUserSelectList(userDB, username) + R"html(
                                    </select>
                                    <select name="filename" class="pwd-input" required>
                                    <option value="">-- Datei auswählen --</option>
                                    )html" + AdvancedFeatures::generateFileSelectList(username) + R"html(
                                        </select>
                                        <button type="submit" class="btn-pwd">Datei senden</button>
                                        </form>
                                        </div>
                                        
                                        <!-- Inbox -->
                                        )html" + inboxHTML + R"html(
                                            
                                            <!-- Passwort Ändern -->
                                            <div class="password-section">
                                            <h3>🔐 Passwort ändern</h3>
                                            <form action="/change-password" method="POST">
                                            <input type="password" name="old_password" class="pwd-input" placeholder="Aktuelles Passwort" required>
                                            <input type="password" name="new_password" class="pwd-input" placeholder="Neues Passwort" required>
                                            <button type="submit" class="btn-pwd">Passwort aktualisieren</button>
                                            </form>
                                            </div>
                                            
                                            <div style="margin-top: 25px; text-align: right;">
                                            <a href="/logout" class="btn btn-danger">Abmelden</a>
                                            </div>
                                            </div>
                                            
                                            <script>
                                            function updateFileName(input) {
                                                const fileNameDisplay = document.getElementById('file-name');
                                                if (input.files && input.files.length > 0) {
                                                    fileNameDisplay.textContent = input.files[0].name;
                                                    fileNameDisplay.style.color = '#4ecca3';
                                                } else {
                                                    fileNameDisplay.textContent = 'Keine Datei ausgewählt';
                                                    fileNameDisplay.style.color = '#888';
                                                }
                                            }
                                            </script>
                                            </body>
                                            </html>
                                            )html";
    }
}
#endif
