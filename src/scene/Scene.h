#pragma once
#include "object/GameObject.h"
#include <list>
#include <typeinfo>
#include "../render/DirectX.h"

class Scene
{
public:
	Scene() = default;
	virtual ~Scene()
	{
		Finalize();
	}
	virtual void Initialize()
	{
		Finalize();
	}
	virtual void Update()
	{

		// ステップ1：全オブジェクトの現在位置を保存（ロールバック用）
		for (auto& gameObject : m_GameObjects)
		{
			gameObject->SaveTransform();
		}

		std::vector<GameObject*> toDelete;
		for (auto gameObject : m_GameObjects)
			if (gameObject->IsDeleted()) toDelete.push_back(gameObject);

		for (auto obj : toDelete)
		{
			DeleteGameObject(obj);
		}

		// ステップ2：各オブジェクトが自身で更新（内部で衝突処理を行う）
		for (const auto& gameObject : m_GameObjects)
		{
			gameObject->Update();
		}
	}
	virtual void Draw()
	{
		DX.setDepthEnable(true);
		for (const auto& gameObject : m_GameObjects)
		{
			gameObject->Draw();
		}
	}
	virtual void Finalize()
	{
		for (auto& gameObject : m_GameObjects)
		{
			gameObject->Finalize();
			delete gameObject;
			gameObject = nullptr;
		}
		m_GameObjects.clear();
	}
	class Camera* GetCurrentCamera() { return m_CurrentCamera; }

	template <class T>
	T* AddGameObject()
	{
		T* gameObject = new T();
		m_GameObjects.push_back(gameObject);
		gameObject->Initialize();
		return gameObject;
	}

	template <class T>
	T* GetGameObject()
	{
		for (auto gameObject : m_GameObjects)
		{
			if (typeid(*gameObject).hash_code() == typeid(T).hash_code())
			{
				return dynamic_cast<T*>(gameObject);
			}
		}
		return nullptr;
	}

	void DeleteGameObject(GameObject* obj)
	{
		obj->Finalize();
		m_GameObjects.remove(obj);
		delete obj;
		obj = nullptr;
	}

	const std::list<GameObject*>& GetAllGameObjects() const { return m_GameObjects; }

protected:
	std::list<GameObject*> m_GameObjects;
	class Camera* m_CurrentCamera{};
};