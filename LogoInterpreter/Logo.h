#pragma once

#include<functional>

#include"Parser.h"
#include"LogoDrawer.h"


class Logo
{
public:

	Logo(const std::string& text):
		text(text)
	{
		drawer.setPos(0, 0);
		drawer.setScale(1.f);
	}

	void setPos(const float& x, const float& y)
	{
		drawer.setPos(x, y);
	}

	void execute(const std::vector<Command>& commands);

	void draw();

private:

	LogoDrawer drawer;

	std::vector<std::string> tokens;

	const std::string text;

};

void Logo::execute(const std::vector<Command>& commands)
{
	for (size_t i = 0; i < commands.size(); i++)
	{
		const Command command = commands[i];
		const std::string name = command.name;
		const std::string arg = command.arg;
		if (name == "repeat")
		{
			for (size_t j = 0; j < stoull(arg); j++)
			{
				execute(command.commands);
			}
		}
		else if (name == "fd")
		{
			drawer.drawLine(std::stof(arg));
		}
		else if (name == "bd")
		{
			drawer.drawLine(-stof(arg));
		}
		else if (name == "lt")
		{
			drawer.rotate(std::stof(arg));
		}
		else if (name == "rt")
		{
			drawer.rotate(-std::stof(arg));
		}
		else if (name == "pd")
		{
			drawer.setPen(true);
		}
		else if (name == "pu")
		{
			drawer.setPen(false);
		}
	}
}

void Logo::draw()
{
	Parser parser(text);

	std::vector<Command> commands;

	commands = parser.parse();

	execute(commands);
}