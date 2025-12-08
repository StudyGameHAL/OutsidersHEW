#include "Field.h"
#include "render/Shader.h"
#include "object/Camera.h"

Field::Field() {
	blockModel[0] = ModelLoad("asset/model/cube.fbx");
	for (int i = 0; i < BLOCK_NUM; i++) {
		blocks[i].type = 0;
		blocks[i].pos = Vector3(
			-5.0f + 1.0f * (i % 10),
			-5.0f,
			0.0f + 1.0f * (i / 10)
		);
	}
}

void Field::Update() {

}

void Field::Draw() {
	Camera* camera = GetSceneCamera();
	SHADER.begin();
	Matrix matrix;
	matrix.projection = camera->GetProjectionMatrix();
	matrix.view = camera->GetViewMatrix();
	for (int i = 0; i < BLOCK_NUM; i++) {
		matrix.world = XMMatrixIdentity();
		matrix.world *= XMMatrixTranslation(blocks[i].pos.x, blocks[i].pos.y, blocks[i].pos.z);
		SHADER.setMatrix(matrix);
		ModelDraw(blockModel[blocks[i].type]);
	}
}