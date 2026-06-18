#ifndef DESIGN_ADMIN_H
#define DESIGN_ADMIN_H

#include <string>

namespace UI {
	std::string getAdminDashboard() {
		return R"html(
			<!DOCTYPE html>
			<html lang="de">
			<head>
				<meta charset="UTF-8">
				<title>Admin Central Unit</title>
				<style>
					body { margin: 0; padding: 0; font-family: 'Courier New', monospace; background-color: #0d0202; color: #ff4a4a; }
					.container { position: absolute; top: 50%; left: 50%; transform: translate(-50%, -50%); width: 95%; max-width: 1250px; background: rgba(20, 5, 5, 0.9); border: 1px solid #ff4a4a; padding: 30px; border-radius: 15px; box-shadow: 0 0 25px #ff4a4a; box-sizing: border-box; }
					.grid { display: grid; grid-template-columns: 1fr 1fr; gap: 20px; margin-top: 20px; }
					.card { background: rgba(0, 0, 0, 0.6); border: 1px dashed #ff4a4a; padding: 15px; border-radius: 5px; min-height: 240px; display: flex; flex-direction: column; justify-content: space-between; box-sizing: border-box; }
					.card h2 { margin-top: 0; font-size: 18px; border-bottom: 1px solid #ff4a4a; padding-bottom: 5px; color: #ff4a4a; }

					/* WICHTIG: Monitor-Box muss sichtbar sein und Scrollbar haben */
					.monitor-box { 
						flex-grow: 1; 
						overflow-y: auto; 
						background: rgba(0,0,0,0.4); 
						padding: 10px; 
						color: #fff; 
						border: 1px solid #330000;
						min-height: 100px; /* Mindesthöhe damit man sieht, wenn es leer ist */
					}

					/* Status-Tabelle */
					.status-table { width: 100%; border-collapse: collapse; font-size: 14px; }
					.status-table td { padding: 6px 0; border-bottom: 1px dashed #220a0a; }
					.status-table tr:hover { background-color: rgba(255, 74, 74, 0.1); }
					
					.btn { background: transparent; border: 1px solid #ff4a4a; color: #ff4a4a; padding: 6px 12px; cursor: pointer; font-family: inherit; font-weight: bold; font-size: 12px; transition: all 0.2s; }
					.btn:hover { background: #ff4a4a; color: #000; }
					
					.input-field { background: #000; border: 1px solid #ff4a4a; color: #fff; padding: 8px; font-family: monospace; width: 100%; box-sizing: border-box; margin-bottom: 15px; outline: none; }
					.input-field:focus { border-color: #fff; }
					
					.btn-group { display: flex; gap: 10px; flex-wrap: wrap; margin-top: 10px; }
					</style>
					</head>
					<body>
					<div class="container">
					<h1>[SYSTEM CENTRAL UNIT - ADMINISTRATOR]</h1>
					<p style="color: #888; margin-bottom: 20px;">[SYSTEM STATUS: ACTIVE] [VERSION 3.0]</p>
					
					<div class="grid">
					<!-- Karte 1: Live-Überwachung -->
					<div class="card">
					<h2>Live-Überwachung (Status)</h2>
					<div class="monitor-box" id="onlineList">
					<span style="color: #666;">Verbinde zur Matrix...</span>
					</div>
					</div>
					
					<!-- Karte 2: Dateisystem -->
					<div class="card">
					<h2>Dateisystem aller User</h2>
					<div class="monitor-box" id="fileList">
					<span style="color: #666;">Verbinde zum Dateisystem...</span>
					</div>
					</div>
					
					<!-- Karte 3: Sanktionen -->
					<div class="card">
					<h2>Sanktionen & Befehle</h2>
					<div style="display: flex; flex-direction: column; justify-content: center; height: 100%;">
					<input type="text" id="targetUser" placeholder="Username eingeben..." class="input-field">
					<div class="btn-group">
					<button class="btn" onclick="executeAction('/admin/warn')">Abmahnen</button>
					<button class="btn" onclick="executeAction('/admin/ban')" style="border-color: #ffaa00; color: #ffaa00;">BAN</button>
					<button class="btn" onclick="executeAction('/admin/unban')" style="border-color: #00ffcc; color: #00ffcc;">Freigeben</button>
					<button class="btn" onclick="executeAction('/admin/delete_user')" style="border-color: #ff4a4a; color: #ff4a4a;">LÖSCHEN</button>
					</div>
					</div>
					</div>
					
					<!-- Karte 4: Benutzer anlegen -->
					<div class="card">
					<h2>Benutzer anlegen (Echtzeit)</h2>
					<form action="/admin/create" method="POST" style="display: flex; flex-direction: column; gap: 10px; justify-content: center; height: 100%;">
					<input type="text" name="username" placeholder="Neuer Username" required class="input-field" style="margin:0;">
					<input type="password" name="password" placeholder="Passwort vergeben" required class="input-field" style="margin:0;">
					<button type="submit" class="btn" style="width: 100%; text-transform: uppercase; background: #ff4a4a; color: #000; border: none;">User aktivieren</button>
					</form>
					</div>
					</div>
					
					<br>
					<div style="text-align: right;">
					<a href="/logout" class="btn" style="text-decoration:none;">Logout</a>
					</div>
					</div>
					
					<script>
					// Funktion für Aktionen (Ban, Warn, Delete)
					function executeAction(route) {
						const usernameInput = document.getElementById('targetUser');
						const username = usernameInput.value.trim();
						
						if (!username) {
							alert('Bitte zuerst einen Usernamen eingeben!');
							return;
						}
						
						if (route === '/admin/delete_user' && !confirm(`Möchtest du '${username}' wirklich PERMANENT löschen?\nDies kann nicht rückgängig gemacht werden!`)) {
							return;
						}
						
						fetch(route, {
							method: 'POST',
							headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
							body: 'username=' + encodeURIComponent(username)
						})
						.then(response => {
							if (!response.ok) throw new Error('Server Error: ' + response.status);
							return response.text();
						})
						.then(text => {
							console.log("Action Success:", text);
							usernameInput.value = ''; // Feld leeren
						update(); // Anzeige sofort aktualisieren
						})
						.catch(err => {
							console.error("Action Error:", err);
							alert("Fehler bei der Aktion: " + err.message);
						});
					}
					
					// Funktion zum Aktualisieren der Listen
					function update() {
						// 1. Online Liste laden
						fetch('/admin/api/online')
						.then(res => {
							if (!res.ok) throw new Error('Status ' + res.status);
							return res.text();
						})
						.then(html => {
							const el = document.getElementById('onlineList');
							if (!html || html.trim() === "") {
								el.innerHTML = "<p style='color:#888; font-style:italic;'>Keine Benutzer im System.</p>";
							} else {
								el.innerHTML = html;
							}
							// Debug im Browser Console
							console.log("[Update] Online Liste geladen.");
						})
						.catch(err => {
							console.error("[Update] Fehler bei Online-Liste:", err);
							document.getElementById('onlineList').innerHTML = "<p style='color:red;'>Fehler: " + err.message + "</p>";
						});
						
						// 2. Datei Liste laden
						fetch('/admin/api/files')
						.then(res => {
							if (!res.ok) throw new Error('Status ' + res.status);
							return res.text();
						})
						.then(html => {
							const el = document.getElementById('fileList');
							if (!html || html.trim() === "") {
								el.innerHTML = "<p style='color:#888; font-style:italic;'>Keine Dateien gefunden.</p>";
							} else {
								el.innerHTML = html;
							}
							console.log("[Update] Datei Liste geladen.");
						})
						.catch(err => {
							console.error("[Update] Fehler bei Datei-Liste:", err);
							document.getElementById('fileList').innerHTML = "<p style='color:red;'>Fehler: " + err.message + "</p>";
						});
					}
					
					// Initialer Aufruf beim Laden der Seite
					window.addEventListener('DOMContentLoaded', () => {
						console.log("Admin Dashboard geladen. Starte Update-Intervall...");
						update();
						setInterval(update, 2000); // Alle 2 Sekunden aktualisieren
					});
					</script>
					</body>
					</html>
					)html";
	}
}

#endif
