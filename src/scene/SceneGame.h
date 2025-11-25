#pragma once
#include "Scene.h"
#include "object/camera.h"
#include "object/TestObject.h"

class SceneGame : public Scene
{
public:
	// TODO
	void Initialize() override
	{
		m_CurrentCamera = AddGameObject<Camera>();
		m_CurrentCamera->GetTransform().SetPosition({ 0.0f, 10.0f, 10.0f });

		AddGameObject<TestObject>();
	}
private:
	// TODO:
};
