#include<Aurora/A2D/GameSceneManager.h>

Scene2D::Scene2D(GameSceneManager* manager) :
	gsm(manager)
{
}

Scene2D::~Scene2D()
{
}

GameSceneManager::~GameSceneManager()
{
	while (scenes.size())
	{
		delete scenes.top();
		scenes.pop();
	}
}

GameSceneManager* GameSceneManager::create()
{
	return new GameSceneManager();
}

void GameSceneManager::push(Scene2D* const scene)
{
	if (!scene)
	{
		throw "[class GameSceneManager] scene is nullptr\n";
		return;
	}
	scenes.push(scene);
}

void GameSceneManager::pop()
{
	if (scenes.size())
	{
		delete scenes.top();
		scenes.pop();
	}
}

void GameSceneManager::set(Scene2D* const scene)
{
	if (scenes.size())
	{
		delete scenes.top();
		scenes.pop();
	}
	if (!scene)
	{
		std::cout << "[class GameSceneManager] scene is nullptr!\n";
		return;
	}
	scenes.push(scene);
}

void GameSceneManager::update(const float& dt) const
{
	scenes.top()->update(dt);
}

void GameSceneManager::render(SpriteBatch* const sBatch, PrimitiveBatch* const pBatch) const
{
	scenes.top()->render(sBatch, pBatch);
}

GameSceneManager::GameSceneManager()
{
}
