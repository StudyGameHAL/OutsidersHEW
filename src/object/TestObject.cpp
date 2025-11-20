#include "TestObject.h"
#include "render/model.h"
#include "render/Shader.h"
#include "Camera.h"

#include <DirectXMath.h>
using namespace DirectX;

void TestObject::Initialize()
{
	m_model = ModelLoad("asset/model/cube.fbx");
}

void TestObject::Draw()
{
	auto camera = GetSceneCamera();
	SHADER.begin();
	Matrix matrix{};
	matrix.projection = camera->GetProjectionMatrix();
	matrix.view = camera->GetViewMatrix();
	matrix.world = m_Transform.GetMatrix();
	SHADER.setMatrix(matrix);
	ModelDraw(m_model);
}

void TestObject::Finalize()
{
	ModelRelease(m_model);
}
