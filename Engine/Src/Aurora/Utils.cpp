#include<Aurora/Utils.h>

std::string Utils::File::readAllText(const std::string& filePath)
{
	std::ifstream stream(filePath);
	std::string str;
	if (stream.is_open())
	{
		std::ostringstream ss;
		ss << stream.rdbuf();
		str = ss.str();
		stream.close();
	}
	return str;
}
