#ifndef CAPTCHA_H
#define CAPTCHA_H

#include <string>
#include <cstdlib>

class CaptchaModule {
public:
	static std::string generate() {
		std::string chars = "ABCDEFGHJKLMNOPQRSTUVWXYZ23456789";
		std::string code = "";
		for (int i = 0; i < 5; ++i) {
			code += chars[rand() % chars.length()];
		}
		return code;
	}
};

#endif
