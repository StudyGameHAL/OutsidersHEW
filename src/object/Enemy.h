#pragma once
#include "GameObject.h"


class Enemy : public GameObject
{
private:
	Vector3 velocity = Vector3(0.0f, 0.0f, 0.0f);
	int health = 0;
	Vector3 maxMoveSpeed = Vector3(0.0f, 0.0f, 0.0f);

public:
	Enemy() {};
	virtual ~Enemy() {};

	void Update() override;
	void Draw() override;
};