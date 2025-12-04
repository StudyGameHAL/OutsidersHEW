#include "object/Player.h"
#include "../core/Keyboard.h"
#include "../render/model.h"
#include "../render/Shader.h"
#include "object/Camera.h"

#define PI			(3.14159265359)
#define TWO_PI		(2 * PI)

void Player::HandleInput()
{
	if (Keyboard_IsKeyDown(KK_A))
	{
		inputVelocity.x += -1.0f;
	}
	if (Keyboard_IsKeyDown(KK_D))
	{
		inputVelocity.x += 1.0f;
	}
	if (Keyboard_IsKeyDown(KK_W))
	{
		inputVelocity.z += 1.0f;
	}
	if (Keyboard_IsKeyDown(KK_S))
	{
		inputVelocity.z += -1.0f;
	}
	if (inputVelocity.x != 0.0f || inputVelocity.z != 0.0f)
	{
		inputVelocity.Normalize();
		SetState(PlayerState::MOVING);
	}
	else
		SetState(PlayerState::IDLE);

	if (Keyboard_IsKeyTrigger(KK_SPACE))
	{
		SetState(PlayerState::DASHING);

	}

}

void Player::Update()
{
	HandleInput();

	float deltaTime = 0.016f;


	if (inputVelocity.x != 0.0f || inputVelocity.z != 0.0f)
	{
		if (currentState == PlayerState::MOVING)
		{

			float inputRotation = atan2f(-inputVelocity.x, -inputVelocity.z);

			if (inputRotation != m_Transform.GetRotationEuler().y)
			{
				Vector3 buffer = m_Transform.GetRotationEuler();
				buffer.y = std::fmod(buffer.y, TWO_PI);

				if (buffer.y > PI)
				{
					buffer.y -= TWO_PI;
				}
				else if (buffer.y < -PI)
				{
					buffer.y += TWO_PI;
				}

				float rotationDiff = inputRotation - buffer.y;
				if (rotationDiff > PI)
					rotationDiff -= TWO_PI;
				else if (rotationDiff < -PI)
					rotationDiff += TWO_PI;
				float rotationStep = 10 * deltaTime;
				if (abs(rotationDiff) <= rotationStep)
					buffer.y = inputRotation;
				else
					buffer.y += (rotationDiff > 0.0f ? rotationStep : -rotationStep);

				m_Transform.SetRotationEuler(buffer);
			}

		}



		velocity.x += inputVelocity.x * moveSpeed * deltaTime;
		velocity.z += inputVelocity.z * moveSpeed * deltaTime;
		inputVelocity = { 0.0f, 0.0f, 0.0f }; // Reset input velocity after applying movement
	}

	if (currentState == PlayerState::DASHING)
	{
		velocity.x = cos(m_Transform.GetRotationEuler().y + PI/2) * 500.f;
		velocity.z = sin(m_Transform.GetRotationEuler().y - PI/2) * 500.f;
	}

	if (velocity.x != 0.0f || velocity.z != 0.0f)
	{
		velocity.x -= velocity.x * friction * deltaTime;
		velocity.z -= velocity.z * friction * deltaTime;
		if (abs(velocity.x) < 0.01f) velocity.x = 0.0f;
		if (abs(velocity.z) < 0.01f) velocity.z = 0.0f;
		if (velocity.Length() > maxMoveSpeed) velocity = velocity.GetNormalized() * moveSpeed;

		Vector3 buffer = m_Transform.GetPosition();
		buffer.x += velocity.x * deltaTime;
		buffer.z += velocity.z * deltaTime;

		m_Transform.SetPosition(buffer);
	}

	//prevPosition = m_Transform.;

}

void Player::Draw()
{
	auto camera = GetSceneCamera();
	SHADER.begin();
	Matrix matrix{};
	matrix.projection = camera->GetProjectionMatrix();
	matrix.view = camera->GetViewMatrix();
	matrix.world = m_Transform.GetMatrix();
	SHADER.setMatrix(matrix);
	ModelDraw(currentModel);

}

void Player::Initialize()
{
	currentModel = ModelLoad("asset/model/cube.fbx");
}
