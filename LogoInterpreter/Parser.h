#pragma once
#include<string>
#include<vector>

#include"Command.h"

class Parser
{
public:

	Parser(const std::string& text):
		text(text)
	{
		index = 0;
	}

	const std::string text;
	size_t index;

	std::vector<Command> parse();
	bool remainingTokens() const;
	std::string getRepeat();
	std::string getNextToken();

};

std::vector<Command> Parser::parse()
{
	std::vector<Command> commands;

	while (remainingTokens())
	{
		const std::string token = getNextToken();

		if (token == "fd" || token == "bd" || token == "lt" || token == "rt")
		{
			commands.push_back(Command(token, getNextToken()));
		}
		else if (token == "pd" || token == "pu")
		{
			commands.push_back(Command(token));
		}
		else if (token == "repeat")
		{
			Command cmd(token, getNextToken());
			Parser parser(getRepeat());
			cmd.commands = parser.parse();
			commands.push_back(cmd);
		}
	}
	return commands;
}

std::string Parser::getRepeat()
{
	while (text.at(index++) != '[' && remainingTokens()) {}
	size_t start = index;
	size_t bracketCount = 1;
	while (bracketCount > 0)
	{
		char ch = text.at(index++);
		if (ch == '[')
		{
			bracketCount++;
		}
		else if (ch == ']')
		{
			bracketCount--;
		}
	}
	size_t end = index;

	return text.substr(start, end - start - 1);

}

bool Parser::remainingTokens() const
{
	return index < text.size();
}

std::string Parser::getNextToken()
{
	std::string token = "";
	char ch = text.at(index);
	if (ch == ' ') {
		index++;
		return getNextToken();
	}
	if (ch == '[' || ch == ']')
	{
		index++;
	}
	while (ch != ' ')
	{
		token.push_back(ch);
		if (++index < text.size())
			ch = text.at(index);
		else {
			break;
		}
	}

	return token;
}