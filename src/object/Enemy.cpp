#include "src/object/Enemy.h"
#include "render/DirectX.h"
#include "src/scene/Scene.h"

class Player;

void Enemy::Update()
{
	Scene* scene = GetScene();
	// GameObject* player = scene->GetGameObject<Player>();
	GameObject* player;
	XMVECTOR playerPosVec = player->GetTransform().GetPosition().ToXMVECTOR();
	XMVECTOR posVec = player->GetTransform().GetPosition().ToXMVECTOR();

	XMMATRIX matrix = XMMatrixLookAtLH(posVec, playerPosVec, XMVectorSet(0.0f, 1.0f, 0.0f));


}

void Enemy::Draw()
{

}