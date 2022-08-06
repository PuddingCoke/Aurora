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

std::string Utils::File::getExtension(const std::string& fileName)
{
	size_t idx = fileName.find_last_of('.');
	return fileName.substr(idx + 1, fileName.size() - idx - 1);
}
