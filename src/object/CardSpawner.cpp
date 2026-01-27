#include "CardSpawner.h"
#include "render/model.h"
#include "render/Shader.h"
#include "scene/Scene.h"
#include "Camera.h"
#include "Game.h"
#include "Card.h"

void CardSpawner::Initialize()
{
	// ‰¼‚ÅBUG1‚ðƒ[ƒhB
	m_Model = ModelLoad("asset/model/ball.fbx");
}

void CardSpawner::Update()
{
	if (cardExists == true) return;
	coolCount++;
	if (coolCount > coolCountMax)
	{
		SpawnCard();
	}
}

void CardSpawner::Draw()
{
	auto camera = GetSceneCamera();
	SHADER.begin();
	Matrix matrix{};
	matrix.projection = camera->GetProjectionMatrix();
	matrix.view = camera->GetViewMatrix();
	matrix.world = XMMatrixIdentity();


	// model adjustments
	matrix.world *= XMMatrixScaling(3.5f, 0.1f, 3.5f);
	// model adjustments end

	matrix.world *= m_Transform.GetMatrix();

	SHADER.setMatrix(matrix);
	ModelDraw(m_Model);
}

void CardSpawner::SpawnCard()
{
	coolCount = 0;
	Scene* scene = GetScene();
	Card* card = scene->AddGameObject<Card>();
	card->GetTransform().SetPosition({ m_Transform.GetPosition().x, m_Transform.GetPosition().y + 0.5f, m_Transform.GetPosition().z });
	card->RegisterSpawner(this);
}
