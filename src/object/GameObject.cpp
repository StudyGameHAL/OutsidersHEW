#include "GameObject.h"
#include "Game.h"
#include "scene/Scene.h"
#include "Camera.h"

Scene* GameObject::GetScene()
{
	return Game::GetInstance()->GetScene();
}

Camera* GameObject::GetSceneCamera()
{
	Scene* scene = GetScene();
	if (!scene)
	{
		return nullptr;
	}
	return scene->GetCurrentCamera();
}
