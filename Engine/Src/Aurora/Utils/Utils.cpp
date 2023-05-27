#include<Aurora/Utils/Utils.h>

std::string Utils::exeRootPath;

std::string Utils::File::backslashToSlash(const std::string& filePath)
{
	std::string result = filePath;

	std::replace(result.begin(), result.end(), '\\', '/');

	return result;
}

std::string Utils::File::getParentFolder(const std::string& filePath)
{
	size_t idx = filePath.find_last_of('\\');

	if (idx == std::string::npos)
	{
		idx = filePath.find_last_of('/');

		if (idx == std::string::npos)
		{
			return "";
		}
	}

	return filePath.substr(0, idx + 1);
}

std::string Utils::File::getExtension(const std::string& filePath)
{
	size_t idx = filePath.find_last_of('.');
	return filePath.substr(idx + 1, filePath.size() - idx - 1);
}

const std::string& Utils::getRootFolder()
{
	return exeRootPath;
}
