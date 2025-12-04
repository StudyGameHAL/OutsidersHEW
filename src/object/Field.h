#pragma once

#include "GameObject.h"
#include "render/model.h"

#define BLOCK_NUM	(100)

struct Block {
	Vector3 pos;
	int type;
};

class Field : public GameObject {
	public:
		Field();
		void Update() override;
		void Draw() override;

	private:
		MODEL* blockModel[1];
		Block blocks[BLOCK_NUM];
};