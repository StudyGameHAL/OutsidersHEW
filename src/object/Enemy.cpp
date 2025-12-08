#include "object/Enemy.h"
#include "render/DirectX.h"
#include "scene/Scene.h"
#include "../render/Shader.h"
#include "object/Camera.h"
#include "object/Player.h"

class Projectile;

void Enemy::Initialize()
{
	m_Model = ModelLoad("asset/model/BUG1.fbx");
}

void Enemy::Update()
{
	Scene* scene = GetScene();
	ApplyRotationToPlayer();
	// アタックカウントを加算
	m_AttackCount++;

	// アタックするカウントになったとき
	if (m_CountToAttack)
	{
		// シーンにゲームオブジェクトを追加
		//scene->AddGameObject<Projectile>();

		// プロジェクタイルに力を加える
	}
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
