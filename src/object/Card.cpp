#include "Card.h"
#include "scene/Scene.h"
#include "render/Shader.h"
#include "Camera.h"
#include "Player.h"
#include "object/CardSpawner.h"

void Card::Initialize()
{
	m_Model = ModelLoad("asset/model/tree.fbx");

	// ===== Capsuleコライダーを追加 =====
	auto collider = MakeSphereCollider(1.0f);
	AddCollider(std::move(collider));
}

void Card::Update()
{
	// ===== TransformをColliderに同期 =====
	SyncCollidersFromTransform();

	// ===== 基底クラスの衝突検出を呼び出し =====
	CheckCollisions();

}

void Card::Draw()
{
	auto camera = GetSceneCamera();
	SHADER.begin();
	Matrix matrix{};
	matrix.projection = camera->GetProjectionMatrix();
	matrix.view = camera->GetViewMatrix();
	matrix.world = m_Transform.GetMatrix();
	SHADER.setMatrix(matrix);
	ModelDraw(m_Model);
}

bool Card::OnCollision(GameObject* other, ColliderBase* myCollider, ColliderBase* otherCollider, const OverlapResult& result)
{
	Scene* scene = GetScene();

	Player* player = scene->GetGameObject<Player>();

	// プレイヤーだった時の処理
	if (other == player)
	{
		SetDeleted(true);
		// spawnerが作ったCardが消えた時の処理を実行。
		m_Spawner->OnCardDeleted();
	}

	// 静的オブジェクト（壁・地面）に接触：ロールバック
	if (other->IsKinematic())
	{
		return true;  // ロールバックが必要
	}

	// その他の場合：処理なし
	return false;
}
