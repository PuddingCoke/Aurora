#pragma once

#include<string>
#include<vector>

struct Command
{
	Command(const std::string& name, const std::string& arg = ""):
		name(name),arg(arg)
	{
	}

	const std::string name;

	const std::string arg;

	std::vector<Command> commands;
};