#pragma once

#ifndef _UTILS_H_
#define _UTILS_H_

#define NOMINMAX

#include<string>
#include<fstream>
#include<sstream>
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

		static std::string readAllText(const std::string& filePath);

		static std::string getExtension(const std::string& fileName);
	};

private:

	friend class Aurora;

	static void ini();

	static std::string exeRootPath;

};


#endif // !_UTILITY_H_