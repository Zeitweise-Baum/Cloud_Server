#ifndef DESIGN_LOGIN_H
#define DESIGN_LOGIN_H

#include <string>

namespace UI {
	std::string getLoginPage(std::string captchaCode) {
		return R"html(
			<!DOCTYPE html>
			<html lang="de">
			<head>
				<meta charset="UTF-8">
				<title>Terminal Access</title>
				<style>
					body { margin: 0; padding: 0; background: #0a0f1d; font-family: 'Courier New', monospace; color: #4ecca3; display: flex; justify-content: center; align-items: center; height: 100vh; }
					.login-box { border: 1px solid #4ecca3; padding: 30px; border-radius: 10px; background: rgba(10,20,40,0.6); box-shadow: 0 0 15px #4ecca3; text-align: center; }
					input { display: block; margin: 10px auto; padding: 8px; border: 1px solid #4ecca3; background: #000; color: #fff; border-radius: 4px; font-family: inherit; }
					.captcha { background: #4ecca3; color: #000; font-weight: bold; padding: 5px; letter-spacing: 3px; margin: 10px 0; }
					button { background: #4ecca3; color: #0a0f1d; border: none; padding: 10px 20px; border-radius: 4px; cursor: pointer; font-weight: bold; }
				</style>
			</head>
			<body>
				<div class="login-box">
					<h2>SECURE TERMINAL LOGIN</h2>
					<form action="/login" method="POST">
						<input type="text" name="username" placeholder="Benutzername" required>
						<input type="password" name="password" placeholder="Passwort" required>
						<div class="captcha">)html" + captchaCode + R"html(</div>
						<input type="text" name="captcha_input" placeholder="Captcha eingeben" required>
						<button type="submit">Login</button>
						</form>
						</div>
						</body>
						</html>
						)html";
	}
}
#endif
