#include "NormalCard.h"
#include "scene/Scene.h"
#include "Player.h"

void NormalCard::Initialize()
{
	Projectile::Initialize();

	// ===== Capsuleコライダーを追加 =====
	auto collider = MakeSphereCollider(1.0f);
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
	Scene* scene = GetScene();

	Player* player = scene->GetGameObject<Player>();
	
	// プレイヤーだった時の処理
	if (other == player)
	{
		SetDeleted(true);

		//// プレイヤーがダッシュ中だったら
		//if (player->GetState() == PlayerState::DASHING)
		//{
		//	return false;
		//}
	}

	// 静的オブジェクト（壁・地面）に接触：ロールバック
	if (other->IsKinematic())
	{
		return true;  // ロールバックが必要
	}
	
	// その他の場合：処理なし
	return false;
}
