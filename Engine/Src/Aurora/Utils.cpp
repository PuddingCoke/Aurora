#include<Aurora/Utils.h>

std::string Utils::exeRootPath;

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

const std::string& Utils::getRootFolder()
{
	return exeRootPath;
}

void Utils::ini()
{
	//获得exe的根目录
	{
		char filePath[2048] = {};
		GetModuleFileNameA(nullptr, filePath, 2048);
		exeRootPath = filePath;

		auto replaceFunc = [](std::string str)->std::string
		{
			for (char& ch : str)
			{
				if (ch == '\\')
					ch = '/';
			}
			return str;
		};
		exeRootPath = replaceFunc(exeRootPath);
		exeRootPath = exeRootPath.substr(0, exeRootPath.find_last_of('/') + 1);
	}
}
