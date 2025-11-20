#pragma once
#include "object/GameObject.h"
#include <list>

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;
	virtual void Load() {};
	virtual void Update()
	{
		for (const auto& gameObject : m_GameObject)
		{
			gameObject->Update();
		}
	}
	virtual void Draw()
	{
		for (const auto& gameObject : m_GameObject)
		{
			gameObject->Draw();
		}
	}
	virtual void Finalize()
	{
		for (const auto& gameObject : m_GameObject)
		{
			gameObject->Draw();
		}
	}
	class Camera* GetCurrentCamera() { return m_CurrentCamera; }

	template <class T>
	T* AddGameObject()
	{
		T* gameObject = new T();
		gameObject->Initialize();
		m_GameObject.push_back(gameObject);
		return gameObject;
	}

	template <class T>
	T* GetGameObject()
	{
		for (auto& gameObject : m_GameObject)
		{
			if (typeid(*gameObject) == typeid(T))
			{
				return dynamic_cast<T*>(gameObject.get());
			}
		}
		return nullptr;
	}
protected:
	std::list<GameObject *> m_GameObject;
	class Camera* m_CurrentCamera{};
};