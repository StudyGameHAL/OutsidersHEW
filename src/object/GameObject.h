#pragma once
#include "Pch.h"
#include "core/Transform.h"
#include "physics/Collider.hpp"
#include <vector>
#include <memory>

class GameObject
{
public:
	GameObject() {};
	virtual ~GameObject() {};
	virtual void Initialize() {};
	virtual void Update() {};
	virtual void Draw() {};
	virtual void Finalize() {}
	const Transform& GetTransform() const { return m_Transform; }
	Transform& GetTransform() { return m_Transform; }

	// ===== コライダー管理 =====
	void AddCollider(std::unique_ptr<ColliderBase> collider);
	const std::vector<std::unique_ptr<ColliderBase>>& GetColliders() const { return m_Colliders; }
	bool HasColliders() const { return !m_Colliders.empty(); }

	// ===== 位置履歴（ロールバック用） =====
	void SaveTransform() { m_PreviousTransform = m_Transform; }
	void RestoreTransform() { m_Transform = m_PreviousTransform; }
	const Transform& GetPreviousTransform() const { return m_PreviousTransform; }

	// ===== TransformをColliderに同期 =====
	void SyncCollidersFromTransform();

	// ===== Kinematicフラグ（静的オブジェクトは押されない） =====
	void SetKinematic(bool kinematic) { m_IsKinematic = kinematic; }
	bool IsKinematic() const { return m_IsKinematic; }

	// ===== 衝突検出（基底クラスがデフォルト実装を提供） =====
	virtual void CheckCollisions();

protected:
	class Scene* GetScene();
	class Camera* GetSceneCamera();
	Transform m_Transform;

	// ===== 衝突コールバック（サブクラスでオーバーライドしてカスタムロジックを実装） =====
	// trueを返すとロールバック、falseは非ロールバック（例：Trigger）
	virtual bool OnCollision(GameObject* other, ColliderBase* myCollider,
		ColliderBase* otherCollider, const OverlapResult& result);
	Transform m_PreviousTransform;  // 前フレームの位置
	std::vector<std::unique_ptr<ColliderBase>> m_Colliders;
	bool m_IsKinematic = false;  // デフォルトは動的オブジェクト

};
