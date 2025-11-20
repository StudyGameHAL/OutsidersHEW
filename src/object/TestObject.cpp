#include "TestObject.h"
#include "render/model.h"
#include "render/Shader.h"

#include <DirectXMath.h>
using namespace DirectX;

void TestObject::Initialize()
{
	m_model = ModelLoad("asset/model/cube.fbx");
}

void TestObject::Draw()
{
	SHADER.begin();
	Matrix matrix{};
	matrix.projection = XMMatrixPerspectiveFovLH(1.0f,
		1280.0f / 720.0f,
		1.0f, 1000.0f);
	XMFLOAT3 up = XMFLOAT3(0.0f, 1.0f, 0.0f);
	XMFLOAT3 target = XMFLOAT3(0.0f, 0.0f, 0.0f);
	XMFLOAT3 pos = XMFLOAT3(0.0f, 10.0f, 10.0f);
	matrix.view = XMMatrixLookAtLH(
		XMLoadFloat3(&pos),
		XMLoadFloat3(&target),
		XMLoadFloat3(&up)
	);
	matrix.world = XMMatrixIdentity();
	SHADER.setMatrix(matrix);
	ModelDraw(m_model);
}

void TestObject::Finalize()
{
	ModelRelease(m_model);
}
