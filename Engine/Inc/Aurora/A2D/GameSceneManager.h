#pragma once

#ifndef _GAMESCENEMANAGER_H_
#define _GAMESCENEMANAGER_H_

#include<stack>

#include"SpriteBatch.h"
#include"PrimitiveBatch.h"

class GameSceneManager;

class Scene2D
{
public:

	Scene2D() = delete;

	Scene2D(const Scene2D&) = delete;

	void operator=(const Scene2D&) = delete;

	Scene2D(GameSceneManager* manager);

	virtual ~Scene2D();

	virtual void handleInput() = 0;

	virtual void update(const float& dt) = 0;

	virtual void render(SpriteBatch* const sBatch, PrimitiveBatch* const pBatch) = 0;

protected:

	GameSceneManager* gsm;

};

class GameSceneManager
{
public:

	GameSceneManager(const GameSceneManager&) = delete;

	void operator=(const GameSceneManager&) = delete;

	~GameSceneManager();

	static GameSceneManager* create();

	void push(Scene2D* const scene);

	void pop();

	void set(Scene2D* const scene);

	void update(const float& dt) const;

	void render(SpriteBatch* const sBatch, PrimitiveBatch* const pBatch) const;

private:

	GameSceneManager();

	std::stack<Scene2D*> scenes;

};


#endif // !_GAMESCENEMANAGER_H_
