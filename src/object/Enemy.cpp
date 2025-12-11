#include "object/Enemy.h"
#include "render/DirectX.h"
#include "scene/Scene.h"
#include "../render/Shader.h"
#include "object/Camera.h"
#include "object/Player.h"
#include "object/Projectile.h"
#include "core/debug_ostream.h"

void Enemy::Initialize()
{
	m_Model = ModelLoad("asset/model/BUG1.fbx");
	
	// ===== Capsuleコライダーを追加 =====
	auto collider = MakeCapsuleCollider(DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }, DirectX::XMFLOAT3{0, 1, 0}, 0.8f);
	AddCollider(std::move(collider));
}

void Enemy::Update()
{
	Scene* scene = GetScene();
	ApplyRotationToPlayer();
	// アタックカウントを加算
	m_AttackFrameCount++;

	// アタックするカウントになったとき
	if (m_AttackFrameCount > m_FrameToAttack)
	{
		m_AttackFrameCount = 0;
		// シーンにゲームオブジェクトを追加
		Projectile* projectile = scene->AddGameObject<Projectile>();

		// プロジェクタイルに力を加える
		projectile->SetVelocity(m_Transform.GetForwardVector() * 2.0f);
	}

	// ===== TransformをColliderに同期 =====
	SyncCollidersFromTransform();

	// ===== 基底クラスの衝突検出を呼び出し =====
	CheckCollisions();
}

void Enemy::Draw()
{
	auto camera = GetSceneCamera();
	SHADER.begin();
	Matrix matrix{};
	matrix.projection = camera->GetProjectionMatrix();
	matrix.view = camera->GetViewMatrix();
	matrix.world = XMMatrixIdentity();

	
	// model adjustments
	matrix.world *= XMMatrixRotationX(-Math::HALF_PI);
	matrix.world *= XMMatrixRotationY(-Math::HALF_PI * 1.0f);
	matrix.world *= XMMatrixScaling(0.7f, 0.7f, 0.7f);
	// model adjustments end

	matrix.world *= m_Transform.GetMatrix();

	SHADER.setMatrix(matrix);
	ModelDraw(m_Model);
}

// ===== 衝突コールバックのオーバーライド：Enemy専用ロジックを実装 =====
bool Enemy::OnCollision(GameObject* other, ColliderBase* myCollider,
	ColliderBase* otherCollider, const OverlapResult& result)
{
	// プレイヤーに接触：ロールバック（重なりを防ぐ）
	if (auto* player = dynamic_cast<Player*>(other))
	{
		hal::dout << "Enemy hit by Player!\n";
		return true;  // ロールバックが必要
	}

	if (!other) return false;

	// 静的オブジェクト（壁・地面）に接触：ロールバック
	if (other->IsKinematic())
	{
		return true;  // ロールバックが必要
	}

	// その他の場合：処理なし
	return false;
}

void Enemy::ApplyRotationToPlayer()
{
	// シーンを取得
	Scene* scene = GetScene();

	// プレイヤーを取得
	GameObject* player = scene->GetGameObject<Player>();

	if (player == nullptr) return;

	// プレイヤーの座標を取得
	Vector3 playerPos = player->GetTransform().GetPosition();

	// 自分自身の座標を取得
	Vector3 myPos = GetTransform().GetPosition();

	// 差分ベクトル（方向）を計算 y軸は除外する
	float dx = playerPos.x - myPos.x;
	float dz = playerPos.z - myPos.z;

	// 角度の計算 (Y軸回転: Yaw)
	// atan2(x, z) は (0,0,1) を基準とした角度をラジアンで返す
	float angleY = atan2(dx, dz);

	// 回転の適用
	m_Transform.SetRotationEuler(Vector3(0.0f, angleY, 0.0f));
}
