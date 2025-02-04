#include "utils.hpp"

std::string readFile(const std::string &filePath) {
	std::ifstream file(filePath);
	if (!file.is_open())
		throw std::runtime_error("Error: Unable to open configuration file: " + filePath);
	std::ostringstream contentStream;
	contentStream << file.rdbuf();
	return (contentStream.str());
}

static std::string trim( std::string& ref, const std::string& trimChars ) {
	ref.erase( 0, ref.find_first_not_of( trimChars ) );
	ref.erase( ref.find_last_not_of( trimChars ) + 1 );
	return ref;
}

std::vector<std::string> split(const std::string &str, char delimiter) {
	std::vector<std::string> result;
	std::string token;
	bool inQuote = false;
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		if (*it == '"')
			inQuote = !inQuote;
		if (*it == delimiter && !inQuote) {
			if (!token.empty()) {
				result.push_back(trim(token, "\"\'"));
				if (!inQuote)
					token.clear();
			}
		} else {
			token += *it;
		}
	} if (!token.empty()) {
		result.push_back(token);
	}
	return (result);
}