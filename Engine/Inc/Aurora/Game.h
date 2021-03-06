#pragma once

#ifndef _GAME_H_
#define _GAME_H_

#include<DirectXMath.h>
#include<DirectXColors.h>

#include"Graphics.h"
#include"Shader.h"

class Game
{
public:

	Game()
	{

	}

	virtual ~Game()
	{

	}

	void operator=(const Game&) = delete;

	virtual void update(const float& dt) = 0;

	virtual void render() = 0;

};

#endif // !_GAME_H_