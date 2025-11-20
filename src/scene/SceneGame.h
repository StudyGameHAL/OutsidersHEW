#pragma once
#include "Scene.h"
#include "object/TestObject.h"

class SceneGame : public Scene
{
public:
	// TODO
	void Load() override
	{
		AddGameObject<TestObject>();
	}
private:
	// TODO:
};
