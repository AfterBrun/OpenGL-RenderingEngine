#include "common.h"

std::optional<std::string> ReadFileString(const char* fileName) {
	std::ifstream file(fileName);
	if (!file.is_open()) {
		SPDLOG_INFO("failed to open file: {}", fileName);
		return {};
	}
	std::stringstream text;
	text << file.rdbuf();
	return text.str();
}