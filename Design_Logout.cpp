#ifndef DESIGN_LOGOUT_H
#define DESIGN_LOGOUT_H

#include <string>

namespace UI {
	std::string getLogoutPage() {
		return R"html(
			<!DOCTYPE html>
			<html lang="de">
			<head>
				<meta charset="UTF-8">
				<title>Terminal - Abgemeldet</title>
				<style>
					body { 
						margin: 0; padding: 0; 
						background: #0a0f1d; font-family: 'Courier New', monospace; 
						color: #4ecca3; display: flex; justify-content: center; align-items: center; 
						height: 100vh;
					}
					.logout-box { 
						border: 1px solid #ff4a4a; padding: 40px; 
						border-radius: 10px; background: rgba(20,10,10,0.6); 
						box-shadow: 0 0 20px rgba(255, 74, 74, 0.3); text-align: center;
						max-width: 500px;
					}
					h2 { color: #ff4a4a; letter-spacing: 2px; margin-bottom: 20px; }
					p { color: #fff; margin-bottom: 30px; line-height: 1.6; }
					.btn { 
						background: transparent; border: 1px solid #4ecca3; 
						color: #4ecca3; padding: 12px 25px; border-radius: 5px; 
						cursor: pointer; font-weight: bold; text-decoration: none;
						display: inline-block;
					}
					.btn:hover { background: #4ecca3; color: #0a0f1d; }
					</style>
					</head>
					<body>
					<div class="logout-box">
					<h2>SITZUNG BEENDET</h2>
					<p>Deine Verbindung zur verschlüsselten Cloud-Zentrale wurde erfolgreich und sicher getrennt.</p>
					<a href="/" class="btn">Erneut anmelden</a>
					</div>
					</body>
					</html>
					)html";
	}
}
#endif
