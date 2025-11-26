#pragma once

//#include "InputManager.h"
//#include "RenderManager.h"
//#include "SoundManager.h"
#include "scene/Scene.h"

class Game
{
public:
	static Game* GetInstance() { return m_Instance; }
	Game();
	~Game();

	void Update();
	void Draw();

	template <class T>
	T* SetNextScene()
	{
		m_SceneNext = new T();
		m_SceneNext->Initialize();
		return dynamic_cast<T *>(m_SceneNext);
	}
	Scene* GetScene() { return m_Scene; }
private:
	void UpdateSceneTransform();
	static Game* m_Instance;
	Scene* m_Scene{ nullptr };
	Scene* m_SceneNext{ nullptr };
};
