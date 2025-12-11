#include "NormalCard.h"

void NormalCard::Initialize()
{
	Projectile::Initialize();

	// ===== Capsuleコライダーを追加 =====
	auto collider = MakeCapsuleCollider(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3{ 0, 1, 0 }, 0.8f);
	AddCollider(std::move(collider));
}

void NormalCard::Update()
{
	Projectile::Update();

	// ===== TransformをColliderに同期 =====
	SyncCollidersFromTransform();

	// ===== 基底クラスの衝突検出を呼び出し =====
	CheckCollisions();

}

bool NormalCard::OnCollision(GameObject* other, ColliderBase* myCollider, ColliderBase* otherCollider, const OverlapResult& result)
{
	return false;
}
