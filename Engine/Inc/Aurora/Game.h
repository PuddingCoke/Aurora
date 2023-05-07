#pragma once

#ifndef _GAME_H_
#define _GAME_H_

class Game
{
public:

	Game()
	{

	}

	virtual ~Game()
	{

	}

	virtual void imGUICall()
	{

	}

	void operator=(const Game&) = delete;

	virtual void update(const float& dt) = 0;

	virtual void render() = 0;

};

#endif // !_GAME_H_