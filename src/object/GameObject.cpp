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

// ===== コライダー管理実装 =====
void GameObject::AddCollider(std::unique_ptr<ColliderBase> collider)
{
	m_Colliders.push_back(std::move(collider));
	SyncCollidersFromTransform();  // 即座に一度同期
}

void GameObject::SyncCollidersFromTransform()
{
	for (auto& collider : m_Colliders)
	{
		auto pos = m_Transform.GetPosition();
		auto rot = m_Transform.GetRotationEuler();
		auto scl = m_Transform.GetScale();

		collider->setOwnerWorldPosition(DirectX::XMFLOAT3{pos.x, pos.y, pos.z});
		collider->setOwnerWorldRotationEuler(DirectX::XMFLOAT3{rot.x, rot.y, rot.z});
		collider->setScale(DirectX::XMFLOAT3{scl.x, scl.y, scl.z});
		collider->updateDerived();
	}
}

// ===== 衝突検出基底クラス実装 =====
void GameObject::CheckCollisions()
{
	if (!HasColliders()) return;

	Scene* scene = GetScene();
	if (!scene) return;

	bool needRestore = false;  // ロールバックが必要かどうか

	// シーン内の全ての他オブジェクトを走査
	for (auto* other : scene->GetAllGameObjects())
	{
		if (other == this) continue;  // 自分自身をスキップ
		if (!other->HasColliders()) continue;

		// thisの各Collider vs otherの各Colliderを検出
		for (auto& myCollider : m_Colliders)
		{
			for (auto& otherCollider : other->GetColliders())
			{
				OverlapResult result;
				if (Intersect(*myCollider, *otherCollider, result))
				{
					// 仮想関数を呼び出してサブクラスに衝突ロジックを処理させる
					bool shouldRestore = OnCollision(other, myCollider.get(),
					                                 otherCollider.get(), result);
					if (shouldRestore)
					{
						needRestore = true;
					}
				}
			}
		}
	}

	// 衝突でロールバックが必要な場合、前フレームの位置に復元
	if (needRestore && !IsKinematic())
	{
		RestoreTransform();
		SyncCollidersFromTransform();
	}
}

// ===== 衝突コールバックのデフォルト実装 =====
bool GameObject::OnCollision(GameObject* other, ColliderBase* myCollider,
                             ColliderBase* otherCollider, const OverlapResult& result)
{
	// デフォルト動作：相手がKinematic（静的オブジェクト）の場合はロールバック
	if (other->IsKinematic())
	{
		return true;  // ロールバックが必要
	}
	return false;  // ロールバック不要
}
