#pragma once

#ifndef _UTILS_H_
#define _UTILS_H_


#include<string>
#include<fstream>
#include<sstream>

class Utils
{
public:

	Utils() = delete;

	Utils(const Utils&) = delete;

	class File
	{
	public:

		File() = delete;

		File(const File&) = delete;

		static std::string readAllText(const std::string& filePath);

		static std::string getExtension(const std::string& fileName);
	};

};


#endif // !_UTILITY_H_