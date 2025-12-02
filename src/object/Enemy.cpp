#include "object/Enemy.h"
#include "render/DirectX.h"
#include "scene/Scene.h"
#include "../render/Shader.h"
#include "object/Camera.h"
#include "object/Player.h"

class Projectile;

void Enemy::Initialize()
{
	m_Model = ModelLoad("asset/model/cube.fbx");
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
	matrix.world = m_Transform.GetMatrix();
	SHADER.setMatrix(matrix);
	ModelDraw(m_Model);
}

void Enemy::ApplyRotationToPlayer()
{
	//Scene* scene = GetScene();

	//GameObject* player = scene->GetGameObject<Player>();
	////GameObject* player;

	//// プレイヤーのベクトルを取得
	//XMVECTOR playerPosVec = player->GetTransform().GetPosition().ToXMVECTOR();
	//// 自分のベクトルを取得
	//XMVECTOR posVec = m_Transform.GetPosition().ToXMVECTOR();

	//// プレイヤーの方向を向くマトリクスを取得
	//XMVECTOR forwardVec = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	//XMVECTOR upVec = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	//XMMATRIX matrix = XMMatrixLookAtLH(posVec, playerPosVec, upVec);

	//XMVECTOR rotation = GetTransform().GetRotationEuler().ToXMVECTOR();

	//XMVECTOR newRotation = XMVector3TransformNormal(rotation, matrix);

	//m_Transform.SetRotationEuler(Vector3(
	//	XMVectorGetX(newRotation),
	//	XMVectorGetY(newRotation),
	//	XMVectorGetZ(newRotation)
	//));

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

	// 必要に応じてラジアンを度数法(Degree)に変換
	// m_Transformがラジアンを受け取るか度数を受け取るかによります
	// float angleYDeg = XMConvertToDegrees(angleY);

	// 4. 回転の適用
	// X軸(Pitch)とZ軸(Roll)は0にして、Y軸だけ回す
	m_Transform.SetRotationEuler(Vector3(0.0f, angleY, 0.0f));
}
