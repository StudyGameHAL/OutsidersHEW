#include "object/Enemy.h"
#include "render/DirectX.h"
#include "scene/Scene.h"

class Player;
class Projectile;

void Enemy::Update()
{
	Scene* scene = GetScene();

	// GameObject* player = scene->GetGameObject<Player>();
	GameObject* player;

	XMVECTOR playerPosVec = player->GetTransform().GetPosition().ToXMVECTOR();
	XMVECTOR posVec = player->GetTransform().GetPosition().ToXMVECTOR();

	XMVECTOR forwardVec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR upVec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMMATRIX matrix = XMMatrixLookAtLH(posVec, playerPosVec, upVec);

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