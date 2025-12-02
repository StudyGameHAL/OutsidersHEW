#include "object/Enemy.h"
#include "render/DirectX.h"
#include "scene/Scene.h"

class Player;
class Projectile;

void Enemy::Update()
{
	Scene* scene = GetScene();

	// アタックカウントを加算
	m_AttackCount++;

	// アタックするカウントになったとき
	if (m_CountToAttack)
	{
		// シーンにゲームオブジェクトを追加
		scene->AddGameObject<Projectile>();

		// プロジェクタイルに力を加える
	}
}

void Enemy::Draw()
{

}

void Enemy::ApplyRotationToPlayer()
{
	Scene* scene = GetScene();

	// GameObject* player = scene->GetGameObject<Player>();
	GameObject* player;

	// プレイヤーのベクトルを取得
	XMVECTOR playerPosVec = player->GetTransform().GetPosition().ToXMVECTOR();
	// 自分のベクトルを取得
	XMVECTOR posVec = player->GetTransform().GetPosition().ToXMVECTOR();

	// プレイヤーの方向を向くマトリクスを取得
	XMMATRIX matrix = XMMatrixLookAtLH(posVec, playerPosVec, XMVectorSet(0.0f, 1.0f, 0.0f, 0,0f));
	XMVECTOR forwardVec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR upVec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX matrix = XMMatrixLookAtLH(posVec, playerPosVec, upVec);

	

	XMVECTOR rotation = GetTransform().GetRotationEuler().ToXMVECTOR();

	XMVECTOR newRotation = XMVector3TransformNormal(rotation, matrix);

	m_Transform.SetRotationEuler(Vector3(
		XMVectorGetX(newRotation),
		XMVectorGetY(newRotation),
		XMVectorGetZ(newRotation)
	));
}
