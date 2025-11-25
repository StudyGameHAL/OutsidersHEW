#pragma once
#include "GameObject.h"
class MODEL;
class TestObject : public GameObject
{
public:
	void Initialize() override;
	void Draw() override;
	void Finalize() override;
private:
	MODEL* m_model;
};