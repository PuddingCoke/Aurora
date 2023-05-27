#pragma once

#ifndef _UTILS_H_
#define _UTILS_H_

#define NOMINMAX

#include<string>
#include<fstream>
#include<sstream>
#include<algorithm>
#include<Windows.h>

class Utils
{
public:

	Utils() = delete;

	Utils(const Utils&) = delete;

	static const std::string& getRootFolder();

	class File
	{
	public:

		File() = delete;

		File(const File&) = delete;

		static std::string backslashToSlash(const std::string& filePath);

		static std::string getParentFolder(const std::string& filePath);

		static std::string getExtension(const std::string& filePath);
	};

private:

	friend class Aurora;

	static std::string exeRootPath;

};

#endif // !_UTILITY_H_