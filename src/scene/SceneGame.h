#pragma once
#include "Scene.h"
#include "object/camera.h"
#include "object/TestObject.h"
#include "object/Player.h"
#include "object/Enemy.h"
#include "object/CardSpawner.h"
#include "object/Field.h"

class SceneGame : public Scene
{
public:
	// TODO
	void Initialize() override
	{
		m_CurrentCamera = AddGameObject<Camera>();
		m_CurrentCamera->GetTransform().SetPosition({ 0.0f, 3.0f, -8.0f });

		// AddGameObject<TestObject>();
		AddGameObject<Player>();
		// AddGameObject<Enemy>();
		AddGameObject<Field>();
		AddGameObject<CardSpawner>()->GetTransform().SetPosition({2.0f, -1.0f, -3.0f});
	}
private:
	// TODO:
};
